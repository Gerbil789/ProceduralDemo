#include "WFC/WFC_DataProcessorActor.h"
#include "WFC/WFC_Utility.h"

AWFC_DataProcessorActor::AWFC_DataProcessorActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComponent);

  static ConstructorHelpers::FObjectFinder<UStaticMesh> WireframeMeshFinder(TEXT("/Game/Wireframe"));
  if (WireframeMeshFinder.Succeeded())
  {
    WireframeMesh = WireframeMeshFinder.Object;
  }
  else
  {
		UE_LOG(LogTemp, Warning, TEXT("Failed to load wireframe mesh."));
  }
}

void AWFC_DataProcessorActor::LoadMeshes()
{
	if (!WFC_Utility::LoadBlocksFromDirectory(MeshDirectoryPath, Blocks, bAddEmptyBlock, bAddFillBlock))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load blocks from directory: %s"), *MeshDirectoryPath);
		return;
	}
}

void AWFC_DataProcessorActor::Preview()
{
  if (Blocks.IsEmpty())
  {
    LoadMeshes();
  }

  Clear();

  // Calculate grid dimensions
  int32 Columns = FMath::CeilToInt(FMath::Sqrt((float)Blocks.Num()));
  int32 Rows = FMath::CeilToInt(static_cast<float>(Blocks.Num()) / Columns);

  FVector GridCenter = FVector(Rows - 1, Columns - 1, 0) * Offset / 2.0f;

  for (int32 i = 0; i < Blocks.Num(); i++)
  {
    int32 Row = i / Columns;
    int32 Col = i % Columns;

    FVector Location = FVector(Offset * Col, Offset * Row, 0) - GridCenter;

    AWFC_ModulePreviewActor* NewActor = GetWorld()->SpawnActor<AWFC_ModulePreviewActor>(Location, FRotator::ZeroRotator);
    if (!NewActor)
    {
      UE_LOG(LogTemp, Error, TEXT("Failed to spawn actor."));
      continue;
    }

		NewActor->Initialize(Blocks[i], WireframeMesh);
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
    NewActor->AttachToActor(this, AttachmentRules);
    NewActor->SetOwner(this);
  }
}

void AWFC_DataProcessorActor::Clear()
{
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);

	for (AActor* AttachedActor : AttachedActors)
	{
		if (AttachedActor)
		{
			AttachedActor->Destroy();
		}
	}
}

void AWFC_DataProcessorActor::SaveBlocks()
{
	if (Blocks.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("No blocks to save."));
		return;
	}

	FString Path = SaveAssetPath + "/" + SaveAssetName;

	if (!WFC_Utility::SaveData(Path, Blocks))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save blocks to: %s"), *Path);
	}
	UE_LOG(LogTemp, Log, TEXT("Saved %d blocks to: %s"), Blocks.Num(), *Path);
}

void AWFC_DataProcessorActor::SaveBlock()
{
	if (!StaticMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("StaticMesh is not set."));
		return;
	}

	FString AssetName = StaticMesh->GetName();

	TArray<FString> Tokens;
	AssetName.ParseIntoArray(Tokens, TEXT("_"), true);

	// Check if the asset name contains the correct number of socket strings
	if (Tokens.Num() < 6)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid number of socket strings in asset name: %s"), *AssetName);
		return;
	}

	TArray<FWFC_Socket> Sockets;
	if (!WFC_Utility::CreateSockets(Tokens, Sockets))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create sockets for mesh: %s"), *AssetName);
		return;
	}

	FWFC_Module NewBlock = FWFC_Module(StaticMesh, Sockets);

	FString Path = SaveAssetPath + "/" + SaveAssetName;
	if (!WFC_Utility::SaveData(Path, NewBlock))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save block to: %s"), *Path);
	}
	UE_LOG(LogTemp, Log, TEXT("Saved block to: %s"), *Path);
}


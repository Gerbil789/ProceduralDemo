#include "WFC/WFC_BlockPreviewActor.h"
#include "WFC/WFC_Utility.h"

AWFC_BlockPreviewActor::AWFC_BlockPreviewActor()
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

void AWFC_BlockPreviewActor::LoadMeshes()
{
	if (!WFC_Utility::LoadBlocksFromDirectory(MeshDirectoryPath, Blocks, bAddEmptyBlock, bAddFillBlock))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load blocks from directory: %s"), *MeshDirectoryPath);
		return;
	}
}

void AWFC_BlockPreviewActor::Preview()
{
  if (Blocks.Num() == 0)
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
    // Compute grid coordinates
    int32 Row = i / Columns;
    int32 Col = i % Columns;

    FVector Location = FVector(Offset * Col, Offset * Row, 0) - GridCenter;
    //FRotator Rotation = FRotator(0, Blocks[i].Rotation, 0);

    AWFC_BlockActor* NewActor = GetWorld()->SpawnActor<AWFC_BlockActor>(Location, FRotator::ZeroRotator);
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

void AWFC_BlockPreviewActor::Clear()
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

void AWFC_BlockPreviewActor::Save()
{
	if (Blocks.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No blocks to save."));
		return;
	}

	if (WFC_Utility::SaveData(SaveAssetPath, Blocks) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save blocks to: %s"), *SaveAssetPath);
	}
	UE_LOG(LogTemp, Warning, TEXT("Saved %d blocks to: %s"), Blocks.Num(), *SaveAssetPath);
}

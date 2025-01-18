#include "WFC/BlockPreviewActor.h"
#include "WFC/Utility.h"


ABlockPreviewActor::ABlockPreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComponent);
}

void ABlockPreviewActor::LoadMeshes()
{
	if (WFC::Utility::LoadBlocksFromDirectory(MeshDirectoryPath, Blocks, bAddEmptyBlock, bAddFillBlock) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load blocks from directory: %s"), *MeshDirectoryPath);
		return;
	}
}

void ABlockPreviewActor::Preview()
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

    ABlockActor* NewActor = GetWorld()->SpawnActor<ABlockActor>(Location, FRotator::ZeroRotator);
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

void ABlockPreviewActor::Clear()
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

#include "InfiniteTerrain.h"


void AInfiniteTerrain::BeginPlay()
{
	Super::BeginPlay();
	LastPlayerPosition = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	UpdateChunks();
}

void AInfiniteTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

  FVector CurrentPlayerPosition = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
  float DistanceMoved = FVector::Dist(CurrentPlayerPosition, LastPlayerPosition);

  // Update chunks only if the player has moved a significant distance
  if (DistanceMoved >= UpdateDistanceThreshold)
  {
    LastPlayerPosition = CurrentPlayerPosition; // Update the last position
    UpdateChunks();
  }
}

void AInfiniteTerrain::UpdateChunks()
{
  int32 PlayerChunkX = FMath::RoundToInt(LastPlayerPosition.X / ((ChunkSize - 1) * QuadSize));
  int32 PlayerChunkY = FMath::RoundToInt(LastPlayerPosition.Y / ((ChunkSize - 1) * QuadSize));

  for (int32 y = -ChunkDistance; y <= ChunkDistance; y++)
  {
    for (int32 x = -ChunkDistance; x <= ChunkDistance; x++)
    {
			int32 ChunkX = PlayerChunkX + x;
			int32 ChunkY = PlayerChunkY + y;

      if (LoadedChunks.Contains(FIntPoint(ChunkX, ChunkY))) continue;
      LoadedChunks.Add(FIntPoint(ChunkX, ChunkY));
      LoadedSectionIndices.Add(FIntPoint(ChunkX, ChunkY), GenerateChunk(ChunkX, ChunkY));
    }
  }

  ClearFarChunks();
  OnChunkGenerated();
}

void AInfiniteTerrain::ClearFarChunks()
{
  TSet<FIntPoint> ChunksToKeep;
  int32 PlayerChunkX = FMath::RoundToInt(LastPlayerPosition.X / ((ChunkSize - 1) * QuadSize));
  int32 PlayerChunkY = FMath::RoundToInt(LastPlayerPosition.Y / ((ChunkSize - 1) * QuadSize));

  for (int32 y = -ChunkDistance; y <= ChunkDistance; y++)
  {
    for (int32 x = -ChunkDistance; x <= ChunkDistance; x++)
    {
      ChunksToKeep.Add(FIntPoint(PlayerChunkX + x, PlayerChunkY + y));
    }
  }

  for (auto It = LoadedChunks.CreateIterator(); It; ++It)
  {
    if (!ChunksToKeep.Contains(*It))
    {
			int32 SectionIndex = LoadedSectionIndices[*It];
      Mesh->ClearMeshSection(SectionIndex);
      LoadedSectionIndices.Remove(*It);
      It.RemoveCurrent();

    }
  }
}



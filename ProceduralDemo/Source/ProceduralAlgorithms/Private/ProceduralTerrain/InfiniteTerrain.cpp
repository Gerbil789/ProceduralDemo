#include "ProceduralTerrain/InfiniteTerrain.h"
#include "ProceduralTerrain/TerrainChunkActor.h"


AInfiniteTerrain::AInfiniteTerrain()
{
  PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void AInfiniteTerrain::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
  LastChunkLocation = GetChunkCoordinates(PlayerPawn->GetActorLocation());


  /*if (TerrainMaterial)
  {
    Mesh->SetMaterial(0, TerrainMaterial);
  }*/

	UpdateChunks(); // Generate initial chunks
}

void AInfiniteTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

  FVector CurrentPlayerLocation = PlayerPawn->GetActorLocation();
	FIntPoint PlayerChunkLocation = GetChunkCoordinates(CurrentPlayerLocation);

  if (PlayerChunkLocation != LastChunkLocation)
  {
		LastChunkLocation = PlayerChunkLocation;
    UpdateChunks();
  }
}

void AInfiniteTerrain::UpdateChunks()
{
  for (int32 y = -RenderRadius; y <= RenderRadius; y++)
  {
    for (int32 x = -RenderRadius; x <= RenderRadius; x++)
    {
      FIntPoint ChunkCoordinates = FIntPoint(LastChunkLocation.X + x, LastChunkLocation.Y + y);

      if (LoadedChunks.Contains(ChunkCoordinates)) continue;

			FVector Location = GetWorldCoordinates(ChunkCoordinates);
      DrawDebugBox(GetWorld(), Location, FVector(50, 50, 50), FColor::Red, false, 0.0f);
      ATerrainChunkActor* Chunk = GetWorld()->SpawnActor<ATerrainChunkActor>(ATerrainChunkActor::StaticClass());
      //Chunk->SetActorLocation(GetWorldCoordinates(ChunkCoordinates));


      LoadedChunks.Add(ChunkCoordinates, Chunk);
      Chunk->GenerateMesh(ChunkCoordinates.X, ChunkCoordinates.Y, this);
      //GenerateChunkAsync(ChunkCoordinates);
    }
  }

  ClearFarChunks();
  OnChunkGenerated();
}

void AInfiniteTerrain::ClearFarChunks()
{
 /* TSet<FIntPoint> ChunksToRemove;

  for (int32 y = -ChunkDistance; y <= ChunkDistance; y++)
  {
    for (int32 x = -ChunkDistance; x <= ChunkDistance; x++)
    {
			FIntPoint Chunk = FIntPoint(PlayerChunk.X + x, PlayerChunk.Y + y);
			if (!LoadedChunks.Contains(Chunk))
			{
				continue;
			}

			if (FMath::Abs(Chunk.X - PlayerChunk.X) > ChunkDistance || FMath::Abs(Chunk.Y - PlayerChunk.Y) > ChunkDistance)
      {
				ChunksToRemove.Add(Chunk);
      }
      
    }
  }

	for (FIntPoint Chunk : ChunksToRemove)
	{
		int32 ChunkId = GetChunkId(Chunk);
    Mesh->ClearMeshSection(ChunkId);
		LoadedChunks.Remove(ChunkId);
	}*/
}

float AInfiniteTerrain::CalculateHeight(float x, float y)
{
  return 0.0f;
}

FIntPoint AInfiniteTerrain::GetChunkCoordinates(FVector WorldLocation)
{
	int32 ChunkX = FMath::RoundToInt(WorldLocation.X / ((ChunkSize - 1) * QuadSize));
	int32 ChunkY = FMath::RoundToInt(WorldLocation.Y / ((ChunkSize - 1) * QuadSize));
	return FIntPoint(ChunkX, ChunkY);

}

FVector AInfiniteTerrain::GetWorldCoordinates(FIntPoint Chunk)
{
	float X = Chunk.X * (ChunkSize - 1) * QuadSize;
	float Y = Chunk.Y * (ChunkSize - 1) * QuadSize;
	return FVector(X, Y, 0);
}



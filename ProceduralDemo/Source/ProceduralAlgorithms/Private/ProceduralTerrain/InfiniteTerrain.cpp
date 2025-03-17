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
  LastChunkLocation = GetChunkCoordinates(FVector2D(PlayerPawn->GetActorLocation()));

  /*if (TerrainMaterial)
  {
    Mesh->SetMaterial(0, TerrainMaterial);
  }*/

	UpdateChunks(); // Generate initial chunks
}

void AInfiniteTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FIntPoint PlayerChunkLocation = GetChunkCoordinates(FVector2D(PlayerPawn->GetActorLocation()));

  if (PlayerChunkLocation != LastChunkLocation)
  {
		LastChunkLocation = PlayerChunkLocation;
    UpdateChunks();
    ClearFarChunks();
		OnChunkGenerated(); // Notify Blueprints
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

			FVector SpawnLocation = FVector(GetWorldCoordinates(ChunkCoordinates), 0.0f);
			FRotator SpawnRotation = FRotator::ZeroRotator;
      USceneComponent* ParentRootComponent = GetRootComponent();

      FActorSpawnParameters SpawnParams;
      SpawnParams.Owner = this;
      SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;


      ATerrainChunkActor* Chunk = GetWorld()->SpawnActor<ATerrainChunkActor>(ATerrainChunkActor::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);

			if (Chunk == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to spawn chunk at %s"), *SpawnLocation.ToString());
				continue;
			}

      Chunk->AttachToComponent(ParentRootComponent, FAttachmentTransformRules::KeepRelativeTransform);

			UE_LOG(LogTemp, Warning, TEXT("Chunk spawned at: %s"), *SpawnLocation.ToString());

      LoadedChunks.Add(ChunkCoordinates, Chunk);
      Chunk->GenerateMesh(ChunkCoordinates, this);
    }
  }
}

void AInfiniteTerrain::ClearFarChunks()
{
  TSet<FIntPoint> ChunksToRemove;

	for (auto& Chunk : LoadedChunks)
	{
		FIntPoint ChunkCoordinates = Chunk.Key;

		//if chunk is over the cleanup radius, remove it
		int AbsX = FMath::Abs(ChunkCoordinates.X - LastChunkLocation.X);
		int AbsY = FMath::Abs(ChunkCoordinates.Y - LastChunkLocation.Y);

		if (AbsX > CleanupRadius || AbsY > CleanupRadius)
		{
			ChunksToRemove.Add(ChunkCoordinates);
		}
	}

	for (FIntPoint ChunkCoordinates : ChunksToRemove)
	{
		LoadedChunks[ChunkCoordinates]->Destroy();
		LoadedChunks.Remove(ChunkCoordinates);

		UE_LOG(LogTemp, Warning, TEXT("Chunk removed at: %s"), *ChunkCoordinates.ToString());
	}
}

//float AInfiniteTerrain::CalculateHeight(FIntPoint ChunkCoordinates, float X, float Y)
//{
//	FVector2D WorldLocation = GetWorldCoordinates(ChunkCoordinates);
//	WorldLocation.X += X * QuadSize;
//	WorldLocation.Y += Y * QuadSize;
//
//	float Height = 0.0f;
//
//	for (UTerrainModifier* Modifier : Modifiers)
//	{
//		if (Modifier && Modifier->bEnabled)
//		{
//			float ModValue = Modifier->GetHeight(WorldLocation);
//
//			Height += Modifier->GetHeight(WorldLocation);
//
//			if (Modifier->ModifierType == ETerrainModifierType::Additive)
//			{
//				Height += ModValue;
//			}
//			else if (Modifier->ModifierType == ETerrainModifierType::Multiplicative)
//			{
//				Height *= ModValue;
//			}
//		}
//	}
//
//	return Height;
//}

void AInfiniteTerrain::ApplyModifiers(FIntPoint ChunkCoordinates, TArray<float>& HeightMap)
{
	FVector2D ChunkWorldPosition = FVector2D(ChunkCoordinates.X * ChunkSize, ChunkCoordinates.Y * ChunkSize);

	for (UTerrainModifier* Modifier : Modifiers)
	{
		if (!Modifier) continue;
		Modifier->ApplyModifier(HeightMap, ChunkSize, ChunkWorldPosition);
	}
}

FIntPoint AInfiniteTerrain::GetChunkCoordinates(FVector2D WorldLocation)
{
	int32 ChunkX = FMath::RoundToInt(WorldLocation.X / (ChunkSize * QuadSize));
	int32 ChunkY = FMath::RoundToInt(WorldLocation.Y / (ChunkSize * QuadSize));
	return FIntPoint(ChunkX, ChunkY);

}

FVector2D AInfiniteTerrain::GetWorldCoordinates(FIntPoint ChunkCoordinates)
{
	float X = ChunkCoordinates.X * ChunkSize * QuadSize;
	float Y = ChunkCoordinates.Y * ChunkSize * QuadSize;
	return FVector2D(X, Y);
}



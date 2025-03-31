#include "ProceduralTerrain/InfiniteTerrain.h"
#include "ProceduralTerrain/TerrainChunkActor.h"
#include "ProceduralTerrain/Modifiers/NoiseModifier.h"
#include "TimerManager.h"

AInfiniteTerrain::AInfiniteTerrain()
{
  PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	//set default lod distances
	LODDistances.Add(3);
	LODDistances.Add(5);
	LODDistances.Add(10);

	UNoiseModifier* NoiseModifier = NewObject<UNoiseModifier>();
	Modifiers.Add(NoiseModifier);
}

void AInfiniteTerrain::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
  LastChunkLocation = GetChunkCoordinates(FVector2D(PlayerPawn->GetActorLocation()));

	CleanUp(); // remove any chunks that might have been spawned in the editor
	InitializeTerrain();
}

void AInfiniteTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FIntPoint PlayerChunkLocation = GetChunkCoordinates(FVector2D(PlayerPawn->GetActorLocation()));

  if (PlayerChunkLocation != LastChunkLocation)
  {
		LastChunkLocation = PlayerChunkLocation;
    UpdateChunks();
	}

	time += DeltaTime;
	if (time < 0.1) return;

	for(int i = 0; i < SpawnLimit; i++)
	{
		if (ChunksQueue.IsEmpty()) return;

		ATerrainChunkActor* Chunk = nullptr;
		ChunksQueue.Dequeue(Chunk);
		if (Chunk)
		{
			int32 LODLevel = CalculateChunkLODLevel(Chunk->Coordinates);
			Chunk->UpdateLODLevel(LODLevel);
		}
	}
	time = 0;
}

void AInfiniteTerrain::Preview()
{
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		UE_LOG(LogTemp, Warning, TEXT("Preview is disabled while the game is running."));
		return;
	}

	CleanUp();
	TArray<FIntPoint> ChunkCoordinatesInRadius = GetChunkCoordinatesInRadius(PreviewRadius);

	for (const FIntPoint& Coordinates : ChunkCoordinatesInRadius)
	{
		FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, FVector(GetWorldCoordinates(Coordinates), 0.0f));

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ATerrainChunkActor* Chunk = GetWorld()->SpawnActor<ATerrainChunkActor>(ATerrainChunkActor::StaticClass(), SpawnTransform, SpawnParams);
		if (Chunk == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn chunk at %s"), *SpawnTransform.GetLocation().ToString());
			continue;
		}

		//Chunk->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		Chunk->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		LoadedChunks.Add(Coordinates, Chunk);

		Chunk->GenerateLODsAsync(Coordinates, this, [this, Chunk]()
			{
				int32 LODLevel = CalculateChunkLODLevel(Chunk->Coordinates);
				Chunk->UpdateLODLevel(LODLevel);
			});
	}
	ChunksQueue.Empty();
}

void AInfiniteTerrain::CleanUp()
{
	UE_LOG(LogTemp, Warning, TEXT("CLEAN UP - Destroying %d chunks"), LoadedChunks.Num());

	for (auto& Chunk : LoadedChunks)
	{
		Chunk.Value->Destroy();
	}
	LoadedChunks.Empty();

	//delete all child actors
	TArray<AActor*> ChildActors;
	GetAttachedActors(ChildActors);

	for (AActor* Actor : ChildActors)
	{
		Actor->Destroy();
	}
}

void AInfiniteTerrain::GenerateHeightmap(FIntPoint ChunkCoordinates, int32 LODLevel, TArray<float>& Heightmap, int32& LODChunkSize, int32& LODQuadSize)
{
	LODChunkSize = GetSize(ChunkSize) / FMath::Pow((float)2, LODLevel);
	LODQuadSize = GetSize(QuadSize) * FMath::Pow((float)2, LODLevel);

	//LODChunkSize = GetSize(ChunkSize);
	//LODQuadSize = GetSize(QuadSize);

	Heightmap.Init(0.0f, (LODChunkSize + 1) * (LODChunkSize + 1));
	ApplyModifiers(ChunkCoordinates, LODChunkSize, Heightmap);
}


void AInfiniteTerrain::InitializeTerrain()
{
	TArray<FIntPoint> ChunkCoordinatesInRenderRadius = GetChunkCoordinatesInRadius(RenderRadius);

	for (FIntPoint& Coordinates : ChunkCoordinatesInRenderRadius)
	{
		// Create an async task for each chunk
		FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, FVector(GetWorldCoordinates(Coordinates), 0.0f));
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ATerrainChunkActor* Chunk = GetWorld()->SpawnActor<ATerrainChunkActor>(ATerrainChunkActor::StaticClass(), SpawnTransform, SpawnParams);
		if (Chunk == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn chunk at %s"), *SpawnTransform.GetLocation().ToString());
			return;
		}

		Chunk->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		LoadedChunks.Add(Coordinates, Chunk);

		Chunk->GenerateLODsAsync(Coordinates, this, [this, Chunk]()
			{
				int32 LODLevel = CalculateChunkLODLevel(Chunk->Coordinates);
				Chunk->UpdateLODLevel(LODLevel);
			});
	}
	ChunksQueue.Empty();
}

void AInfiniteTerrain::UpdateChunks()
{
	TArray<FIntPoint> ChunkCoordinatesInRenderRadius = GetChunkCoordinatesInRadius(RenderRadius);

	for (const FIntPoint& Coordinates : ChunkCoordinatesInRenderRadius)
	{
		if (LoadedChunks.Contains(Coordinates)) continue;

		FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, FVector(GetWorldCoordinates(Coordinates), 0.0f));
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ATerrainChunkActor* Chunk = GetWorld()->SpawnActor<ATerrainChunkActor>(ATerrainChunkActor::StaticClass(), SpawnTransform, SpawnParams);
		if (Chunk == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn chunk at %s"), *SpawnTransform.GetLocation().ToString());
			continue;
		}

		Chunk->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		LoadedChunks.Add(Coordinates, Chunk);

		Chunk->GenerateLODsAsync(Coordinates, this);
	}

	//OnTerrainUpdated.Broadcast();
	ClearFarChunks();

	for (const FIntPoint& Coordinates : ChunkCoordinatesInRenderRadius)
	{
		if (LoadedChunks.Contains(Coordinates))
		{
			ATerrainChunkActor* Chunk = LoadedChunks[Coordinates];
			int32 LODLevel = CalculateChunkLODLevel(Chunk->Coordinates);
			Chunk->UpdateLODLevel(LODLevel);
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
	}
}

void AInfiniteTerrain::ApplyModifiers(FIntPoint ChunkCoordinates, int32 Size, TArray<float>& HeightMap)
{
	FVector2D ChunkWorldPosition = FVector2D(ChunkCoordinates.X * GetSize(ChunkSize), ChunkCoordinates.Y * GetSize(ChunkSize));

	for (UTerrainModifier* Modifier : Modifiers)
	{
		if (!Modifier) continue;
		Modifier->ApplyModifier(HeightMap, Size, ChunkWorldPosition);
	}
}

FIntPoint AInfiniteTerrain::GetChunkCoordinates(FVector2D WorldLocation)
{
	int32 ChunkX = FMath::RoundToInt(WorldLocation.X / (GetSize(ChunkSize) * GetSize(QuadSize)));
	int32 ChunkY = FMath::RoundToInt(WorldLocation.Y / (GetSize(ChunkSize) * GetSize(QuadSize)));
	return FIntPoint(ChunkX, ChunkY);

}

FVector2D AInfiniteTerrain::GetWorldCoordinates(FIntPoint ChunkCoordinates)
{



	float X = ChunkCoordinates.X * GetSize(ChunkSize) * GetSize(QuadSize);
	float Y = ChunkCoordinates.Y * GetSize(ChunkSize) * GetSize(QuadSize);
	return FVector2D(X, Y);
}

TArray<FIntPoint> AInfiniteTerrain::GetChunkCoordinatesInRadius(int Radius)
{
	TArray<FIntPoint> Coordinates;
	for (int32 y = -Radius; y <= Radius; y++)
	{
		for (int32 x = -Radius; x <= Radius; x++)
		{
			Coordinates.Add(FIntPoint(LastChunkLocation.X + x, LastChunkLocation.Y + y));
		}
	}
	return Coordinates;
}

int32 AInfiniteTerrain::CalculateChunkLODLevel(FIntPoint ChunkCoordinates)
{
	for (int32 i = 0; i < LODDistances.Num(); i++)
	{
		int32 DistanceX = FMath::Abs(ChunkCoordinates.X - LastChunkLocation.X);
		int32 DistanceY = FMath::Abs(ChunkCoordinates.Y - LastChunkLocation.Y);

		int32 Distance = FMath::Max(DistanceX, DistanceY);
		if (Distance <= LODDistances[i])
		{
			return i;
		}
	}

	return LODDistances.Num() - 1;

	
}

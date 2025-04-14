#include "ProceduralTerrain/InfiniteTerrain.h"
#include "ProceduralTerrain/TerrainChunkActor.h"
#include "ProceduralTerrain/Modifiers/NoiseModifier.h"
#include "Profiling/ProfilingMacros.h"
#include "Profiling/ProfilerUtils.h"

AInfiniteTerrain::AInfiniteTerrain()
{
  PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	//set default values
	LODDistances.Empty();
	LODDistances.Add(8);
	LODDistances.Add(10);
	LODDistances.Add(12);

	UNoiseModifier* NoiseModifier = NewObject<UNoiseModifier>();
	Modifiers.Add(NoiseModifier);
}

void AInfiniteTerrain::BeginPlay()
{
#if ENABLE_PROFILING // TODO: move this somewhere else
	FProfilerUtils::InitSession(); 
#endif

	Super::BeginPlay();

	if (!bTerrainEnabled) return;

	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	ensure(PlayerPawn); // Ensure that the player pawn is valid
  LastChunkLocation = GetChunkCoordinates(FVector2D(PlayerPawn->GetActorLocation()));

	if(bCleanUpOnBeginPlay)
	{
		CleanUp();
	}

	//initialize modifiers
	for (UTerrainModifier* Modifier : Modifiers)
	{
		if (Modifier)
		{
			Modifier->Initialize();
		}
	}

	InitializeTerrain();
}

void AInfiniteTerrain::Tick(float DeltaTime)
{
	//TERRAIN_SCOPE_TIME(Tick);
	Super::Tick(DeltaTime);

	if (!bTerrainEnabled) return;

	FIntPoint PlayerChunkLocation = GetChunkCoordinates(FVector2D(PlayerPawn->GetActorLocation()));

  if (PlayerChunkLocation != LastChunkLocation)
  {
		LastChunkLocation = PlayerChunkLocation;
		TArray<FIntPoint> ChunkCoordinatesInRenderRadius = GetChunkCoordinatesInRadius(RenderRadius);

		SpawnNewChunks(ChunkCoordinatesInRenderRadius);
		ClearFarChunks();
		UpdateChunksLOD(ChunkCoordinatesInRenderRadius);
		//OnTerrainUpdated.Broadcast();
	}

	ProcessChunksQueue();
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

	//initialize modifiers
	for (UTerrainModifier* Modifier : Modifiers)
	{
		if (Modifier)
		{
			Modifier->Initialize();
		}
	}

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

		Chunk->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		LoadedChunks.Add(Coordinates, Chunk);

		Chunk->GenerateLODsAsync(Coordinates, this, [this, Chunk]()
			{
				int32 LODLevel = CalculateChunkLODLevel(Chunk->Coordinates);
				if (LODLevel != Chunk->GetCurrentLODLevel())
				{
					Chunk->UpdateLODLevel(LODLevel);
				}
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

	//delete all child actors, if any remain from previous session
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

	Heightmap.Init(0.0f, (LODChunkSize + 1) * (LODChunkSize + 1));

	FVector2D ChunkWorldPosition = FVector2D(ChunkCoordinates.X * GetSize(ChunkSize), ChunkCoordinates.Y * GetSize(ChunkSize));

	// Apply modifiers
	for (UTerrainModifier* Modifier : Modifiers)
	{
		if (!Modifier) continue;

		for (int Y = 0; Y <= LODChunkSize; Y++)
		{
			for (int X = 0; X <= LODChunkSize; X++)
			{
				int Index = X + Y * (LODChunkSize + 1);

				int32 Ratio = GetSize(ChunkSize) / LODChunkSize;
				FVector2D Location = FVector2D(ChunkWorldPosition.X + X * Ratio, ChunkWorldPosition.Y + Y * Ratio);
				Modifier->ApplyModifier(Heightmap[Index], Location);
			}
		}
	}


}

void AInfiniteTerrain::InitializeTerrain()
{
	TArray<FIntPoint> ChunkCoordinatesInRenderRadius = GetChunkCoordinatesInRadius(RenderRadius, true);

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
				if (LODLevel != Chunk->GetCurrentLODLevel())
				{
					Chunk->UpdateLODLevel(LODLevel);
				}
			});
	}
	ChunksQueue.Empty();
}

void AInfiniteTerrain::ProcessChunksQueue()
{
	int32 Processed = 0;
	while (!ChunksQueue.IsEmpty() && Processed < SpawnLimit)
	{
		TWeakObjectPtr<ATerrainChunkActor> WeakChunk;
		ChunksQueue.Dequeue(WeakChunk);
		if (!WeakChunk.IsValid())
		{
			++Processed;
			continue;
		}

		ATerrainChunkActor* Chunk = WeakChunk.Get();
		int32 LODLevel = CalculateChunkLODLevel(Chunk->Coordinates);
		Chunk->UpdateLODLevel(LODLevel);
		++Processed;
	}
}

void AInfiniteTerrain::SpawnNewChunks(const TArray<FIntPoint>& ChunkCoordinatesInRenderRadius)
{
	for (const FIntPoint& Coordinates : ChunkCoordinatesInRenderRadius)
	{
		if (LoadedChunks.Contains(Coordinates)) continue;

		FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, FVector(GetWorldCoordinates(Coordinates), 0.0f));
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

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
}

void AInfiniteTerrain::ClearFarChunks()
{
  TSet<FIntPoint> ChunksToRemove;

	for (auto& Chunk : LoadedChunks)
	{
		FIntPoint ChunkCoordinates = Chunk.Key;
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

void AInfiniteTerrain::UpdateChunksLOD(const TArray<FIntPoint>& ChunkCoordinatesInRenderRadius)
{
	// Update LOD levels for loaded chunks
	for (const FIntPoint& Coordinates : ChunkCoordinatesInRenderRadius)
	{
		if (LoadedChunks.Contains(Coordinates))
		{
			TWeakObjectPtr<ATerrainChunkActor> WeakChunk(LoadedChunks[Coordinates]);

			if (!WeakChunk.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("Weak chunk is invalid for coordinates %s"), *Coordinates.ToString());
				continue;
			}

			int32 LODLevel = CalculateChunkLODLevel(WeakChunk->Coordinates);
			if (LODLevel != WeakChunk->GetCurrentLODLevel())
			{
				ChunksQueue.Enqueue(WeakChunk);
			}
		}
	}
}

FIntPoint AInfiniteTerrain::GetChunkCoordinates(const FVector2D& WorldLocation)
{
	int32 ChunkX = FMath::RoundToInt(WorldLocation.X / (GetSize(ChunkSize) * GetSize(QuadSize)));
	int32 ChunkY = FMath::RoundToInt(WorldLocation.Y / (GetSize(ChunkSize) * GetSize(QuadSize)));
	return FIntPoint(ChunkX, ChunkY);
}

FVector2D AInfiniteTerrain::GetWorldCoordinates(const FIntPoint& ChunkCoords)
{
	float X = ChunkCoords.X * GetSize(ChunkSize) * GetSize(QuadSize);
	float Y = ChunkCoords.Y * GetSize(ChunkSize) * GetSize(QuadSize);
	return FVector2D(X, Y);
}

TArray<FIntPoint> AInfiniteTerrain::GetChunkCoordinatesInRadius(int32 Radius, bool Sort)
{
	TArray<FIntPoint> Coordinates;
	for (int32 Y = -Radius; Y <= Radius; Y++)
	{
		for (int32 X = -Radius; X <= Radius; X++)
		{
			Coordinates.Add(FIntPoint(LastChunkLocation.X + X, LastChunkLocation.Y + Y));
		}
	}

	if(Sort)
	{
		// Sort by distance to the last chunk location (chunks close to player first)
		Coordinates.Sort([this](const FIntPoint& A, const FIntPoint& B)
			{
				int32 DistA = FMath::Abs(A.X - LastChunkLocation.X) + FMath::Abs(A.Y - LastChunkLocation.Y);
				int32 DistB = FMath::Abs(B.X - LastChunkLocation.X) + FMath::Abs(B.Y - LastChunkLocation.Y);

				return DistA < DistB;
			});
	}

	return Coordinates;
}

int32 AInfiniteTerrain::CalculateChunkLODLevel(const FIntPoint& ChunkCoords)
{
	for (int32 i = 0; i < LODDistances.Num(); i++)
	{
		int32 DistanceX = FMath::Abs(ChunkCoords.X - LastChunkLocation.X);
		int32 DistanceY = FMath::Abs(ChunkCoords.Y - LastChunkLocation.Y);

		int32 Distance = FMath::Max(DistanceX, DistanceY);
		if (Distance <= LODDistances[i])
		{
			return i;
		}
	}
	return LODDistances.Num() - 1;
}

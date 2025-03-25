#include "ProceduralTerrain/InfiniteTerrain.h"
#include "ProceduralTerrain/TerrainChunkActor.h"
#include "TimerManager.h"

//TODO: move it into some utils
void LogTime(FString message)
{
	FDateTime Now = FDateTime::Now();
	FString TimeString = Now.ToString(TEXT("%Y-%m-%d %H:%M:%S:%s"));
	UE_LOG(LogTemp, Warning, TEXT("[%s] %s"), *TimeString, *message);
}

AInfiniteTerrain::AInfiniteTerrain()
{
  PrimaryActorTick.bCanEverTick = true;
	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
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

	for(int i = 0; i < SpawnLimit; i++)
	{
		if (ChunksQueue.IsEmpty()) return;

		ATerrainChunkActor* Chunk = nullptr;
		ChunksQueue.Dequeue(Chunk);
		if (Chunk)
		{
			Chunk->SpawnMesh();
		}
	}
}

void AInfiniteTerrain::Preview()
{
	CleanUp();
	FIntPoint Coordinates = FIntPoint(0, 0);
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
	Chunk->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	LoadedChunks.Add(Coordinates, Chunk);

	Chunk->GenerateMeshDataAsync(Coordinates, this, [this, Chunk]()
		{
			Chunk->SpawnMesh();
		});
}

void AInfiniteTerrain::CleanUp()
{
	UE_LOG(LogTemp, Warning, TEXT("CLEAN UP - Destroying %d chunks"), LoadedChunks.Num());
	for (auto& Chunk : LoadedChunks)
	{
		Chunk.Value->Destroy();
	}
	LoadedChunks.Empty();
}


void AInfiniteTerrain::InitializeTerrain()
{
	FGraphEventArray Tasks;
	TArray<FIntPoint> ChunkCoordinatesInRenderRadius = GetChunkCoordinatesInRadius();

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

		Chunk->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		LoadedChunks.Add(Coordinates, Chunk);

		Chunk->GenerateMeshDataAsync(Coordinates, this, [this, Chunk]()
			{
				Chunk->SpawnMesh();
			});
	}
}

void AInfiniteTerrain::UpdateChunks()
{
	TArray<FIntPoint> ChunkCoordinatesInRenderRadius = GetChunkCoordinatesInRadius();

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

		Chunk->GenerateMeshDataAsync(Coordinates, this, [this, Chunk]()
			{
				ChunksQueue.Enqueue(Chunk);
			});
	}

	//OnTerrainUpdated.Broadcast();
	ClearFarChunks();
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

TArray<FIntPoint> AInfiniteTerrain::GetChunkCoordinatesInRadius()
{
	TArray<FIntPoint> Coordinates;
	for (int32 y = -RenderRadius; y <= RenderRadius; y++)
	{
		for (int32 x = -RenderRadius; x <= RenderRadius; x++)
		{
			Coordinates.Add(FIntPoint(LastChunkLocation.X + x, LastChunkLocation.Y + y));
		}
	}
	return Coordinates;
}




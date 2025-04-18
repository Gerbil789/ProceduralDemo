#include "ProceduralTerrain/InfiniteTerrain.h"
#include "ProceduralTerrain/TerrainChunk.h"
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
#if ENABLE_PROFILING // TODO: move profiler initialization somewhere else
	FProfilerUtils::InitSession(); 
#endif

	Super::BeginPlay();
	SetActorTickEnabled(bTerrainEnabled);

	if (!bTerrainEnabled) return;

	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	ensure(PlayerPawn); // Ensure that the player pawn is valid
  LastChunkLocation = GetChunkCoordinates(FVector2D(PlayerPawn->GetActorLocation()));

	if (bCleanUpOnBeginPlay)
	{
		CleanUp();
	}


	InitializeModifiers();
	InitializeTerrain();
}

void AInfiniteTerrain::Tick(float DeltaTime)
{
	TERRAIN_SCOPE_TIME(Tick);
	Super::Tick(DeltaTime);

	FIntPoint PlayerChunkLocation = GetChunkCoordinates(FVector2D(PlayerPawn->GetActorLocation()));

  if (PlayerChunkLocation != LastChunkLocation)
  {

		LastChunkLocation = PlayerChunkLocation;
		UE_LOG(LogTemp, Warning, TEXT("Player chunk location: %s"), *PlayerChunkLocation.ToString());
		TArray<FIntPoint> ChunkCoordinatesInRenderRadius = GetChunkCoordinatesInRadius(RenderRadius);

		SpawnNewChunks(ChunkCoordinatesInRenderRadius);
		ClearFarChunks();
		UpdateChunks(ChunkCoordinatesInRenderRadius);
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

	InitializeModifiers();
	CleanUp();
	InitializeTerrain();
}

void AInfiniteTerrain::CleanUp()
{
	UE_LOG(LogTemp, Warning, TEXT("CLEAN UP - Destroying %d chunks"), LoadedChunks.Num());

	for (auto& Mesh : MeshComponents)
	{
		if (Mesh.Value)
		{
			Mesh.Value->DestroyComponent();
		}
	}

	MeshComponents.Empty();
	LoadedChunks.Empty();
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
		if (LoadedChunks.Contains(Coordinates)) continue;

		int32 LODLevel = CalculateLODLevel(Coordinates);
		TSharedPtr<TerrainChunk> Chunk = MakeShared<TerrainChunk>(Coordinates, this, LODLevel);

		int LODCount = LODDistances.Num();
		Chunk->GenerateMeshDataAsync([this, Chunk]()
			{
				UpdateMeshComponent(Chunk);
				LoadedChunks.Add(Chunk->Coordinates, Chunk);
			});
	}
	ChunksQueue.Empty();
}

void AInfiniteTerrain::InitializeModifiers()
{
	for (UTerrainModifier* Modifier : Modifiers)
	{
		if (Modifier)
		{
			Modifier->Initialize();
		}
	}
}

void AInfiniteTerrain::ProcessChunksQueue()
{
	int32 Processed = 0;
	while (!ChunksQueue.IsEmpty() && Processed < SpawnLimit)
	{
		TWeakPtr<TerrainChunk> Chunk;
		ChunksQueue.Dequeue(Chunk);
		if (Chunk.IsValid())
		{
			//UE_LOG(LogTemp, Warning, TEXT("Update Chunk: %s"), *Chunk.Pin()->Coordinates.ToString());
			UpdateMeshComponent(Chunk);
		}
		++Processed;
	}

	//log processed

	//if (Processed > 0)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Processed %d chunks"), Processed);
	//}
}

void AInfiniteTerrain::UpdateMeshComponent(TWeakPtr<TerrainChunk> Chunk)
{
	if (Chunk.IsValid() == false) return;
	TerrainChunk* ChunkPtr = Chunk.Pin().Get();

	if (!MeshComponents.Contains(ChunkPtr->Coordinates))
	{
		CreateMeshComponent(Chunk);
		return;
	}

	UProceduralMeshComponent* MeshComponent = MeshComponents[ChunkPtr->Coordinates];

	if (MeshComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Mesh component is null for chunk %s"), *ChunkPtr->Coordinates.ToString());
		return;
	}

	int32 NumSections = MeshComponent->GetNumSections();
	int32 LODLevel = ChunkPtr->GetCurrentLODLevel();

	for (int32 i = 0; i < NumSections; ++i)
	{
		bool bVisible = (i == LODLevel);
		MeshComponent->SetMeshSectionVisible(i, bVisible);
	}
}

void AInfiniteTerrain::CreateMeshComponent(TWeakPtr<TerrainChunk> Chunk)
{
	if (Chunk.IsValid() == false) return;
	TerrainChunk* ChunkPtr = Chunk.Pin().Get();

	UProceduralMeshComponent* MeshComponent = NewObject<UProceduralMeshComponent>(this);
	MeshComponent->RegisterComponent();
	MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	MeshComponent->SetWorldLocation(FVector(GetWorldCoordinates(ChunkPtr->Coordinates), 0.0f));
	MeshComponents.Add(ChunkPtr->Coordinates, MeshComponent);

	// Create mesh sections for each LOD level
	for (int32 LODLevel = 0; LODLevel < LODDistances.Num(); LODLevel++)
	{
		if (ChunkPtr->MeshData.Contains(LODLevel))
		{
			TSharedPtr<FMeshData> MeshData = ChunkPtr->MeshData[LODLevel];
			MeshComponent->CreateMeshSection(LODLevel, MeshData->Vertices, MeshData->Triangles, MeshData->Normals, MeshData->UVs, TArray<FColor>(), MeshData->Tangents, true);
			MeshComponent->SetMeshSectionVisible(LODLevel, false);

			if (TerrainMaterial)
			{
				MeshComponent->SetMaterial(LODLevel, TerrainMaterial);
			}
		}
	}

	MeshComponent->SetMeshSectionVisible(ChunkPtr->GetCurrentLODLevel(), true);
}

void AInfiniteTerrain::SpawnNewChunks(const TArray<FIntPoint>& ChunkCoordinatesInRenderRadius)
{
	for (const FIntPoint& Coordinates : ChunkCoordinatesInRenderRadius)
	{
		if (LoadedChunks.Contains(Coordinates)) continue;

		int32 LODLevel = CalculateLODLevel(Coordinates);
		TSharedPtr<TerrainChunk> Chunk = MakeShared<TerrainChunk>(Coordinates, this, LODLevel);
		LoadedChunks.Add(Coordinates, Chunk);

		int LODCount = LODDistances.Num();
		Chunk->GenerateMeshDataAsync();
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
		if (MeshComponents.Contains(ChunkCoordinates))
		{
			UProceduralMeshComponent* MeshComponent = MeshComponents[ChunkCoordinates];
			if (MeshComponent)
			{
				MeshComponent->DestroyComponent();
			}
			MeshComponents.Remove(ChunkCoordinates);
		}

		LoadedChunks.Remove(ChunkCoordinates);
	}
}

void AInfiniteTerrain::UpdateChunks(const TArray<FIntPoint>& ChunkCoordinatesInRenderRadius)
{
	for (const FIntPoint& Coordinates : ChunkCoordinatesInRenderRadius)
	{
		if (!LoadedChunks.Contains(Coordinates)) continue;
		TSharedPtr<TerrainChunk> Chunk = LoadedChunks[Coordinates];
		Chunk->UpdateLODLevel();
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

int32 AInfiniteTerrain::CalculateLODLevel(const FIntPoint& ChunkCoords)
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


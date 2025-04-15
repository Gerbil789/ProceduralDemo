#include "ProceduralTerrain/TerrainChunkActor.h"
#include "ProceduralTerrain/InfiniteTerrain.h"
#include "ProceduralTerrain/Enums/TerrainEnums.h"
#include "ProceduralTerrain/MeshStrategies/DefaultStrategy.h"
#include "ProceduralTerrain/MeshStrategies/QuadTreeStrategy.h"
#include "ProceduralTerrain/MeshStrategies/VertexClusteringStrategy.h"
#include "ProceduralTerrain/MeshStrategies/QuadraticErrorMetricsStrategy.h"
#include "Profiling/ProfilingMacros.h"


ATerrainChunkActor::ATerrainChunkActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
	RootComponent = MeshComponent;

	CurrentLODLevel = -1;
}

void ATerrainChunkActor::GenerateLODsAsync(FIntPoint ChunkCoordinates, AInfiniteTerrain* InfiniteTerrain, TFunction<void()> OnComplete)
{
	TWeakObjectPtr<ATerrainChunkActor> WeakThis(this);
	TWeakObjectPtr<AInfiniteTerrain> WeakTerrain(InfiniteTerrain);

	this->Terrain = InfiniteTerrain;
	this->Coordinates = ChunkCoordinates;

	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis, WeakTerrain, OnComplete]()
		{
			if (!WeakThis.IsValid() || !WeakTerrain.IsValid()) return;

			TERRAIN_SCOPE_TIME(GenerateLODsAsync);

			ATerrainChunkActor* Chunk = WeakThis.Get();
			AInfiniteTerrain* Terrain = WeakTerrain.Get();

			for (int32 LOD = 0; LOD < Terrain->LODDistances.Num(); LOD++)
			{
				Chunk->GenerateLODMeshData(LOD);
			}

			// TQueue is thread-safe, we can safely add the chunk to the queue
			Terrain->ChunksQueue.Enqueue(WeakThis);

			AsyncTask(ENamedThreads::GameThread, [WeakThis, OnComplete]()
				{
					if(!WeakThis.IsValid()) return;
					OnComplete();
				});
		});
}

void ATerrainChunkActor::UpdateLODLevel(int32 NewLODLevel)
{
	if (NewLODLevel == CurrentLODLevel) return;

	if (!LODMeshData.Contains(NewLODLevel))
	{
		UE_LOG(LogTemp, Error, TEXT("LOD level %d not found"), NewLODLevel);
		return;
	}

	TERRAIN_SCOPE_TIME(GenerateLODsAsync);

	// Set new LOD level
	bool bCreateCollision = NewLODLevel == 0;
	MeshComponent->CreateMeshSection(0, LODMeshData[NewLODLevel]->Vertices, LODMeshData[NewLODLevel]->Triangles, LODMeshData[NewLODLevel]->Normals, LODMeshData[NewLODLevel]->UVs, TArray<FColor>(), LODMeshData[NewLODLevel]->Tangents, bCreateCollision);
	if (Terrain->TerrainMaterial)
	{
		MeshComponent->SetMaterial(0, Terrain->TerrainMaterial);
	}

	CurrentLODLevel = NewLODLevel;
}

void ATerrainChunkActor::GenerateLODMeshData(int32 LODLevel)
{
	TArray<float> Heightmap;
	int32 LODChunkSize;
	int32 LODQuadSize;

	Terrain->GenerateHeightmap(this->Coordinates, LODLevel, Heightmap, LODChunkSize, LODQuadSize);
	TSharedPtr<FMeshData>	MeshData = MakeShared<FMeshData>();

	// Generate mesh
	switch (Terrain->MeshReductionStrategy)
	{
	case EMeshStrategy::Default:
		DefaultStrategy::GenerateMesh(Heightmap, LODChunkSize, LODQuadSize, *MeshData);
		break;

	case EMeshStrategy::QuadTree:
		QuadTreeStrategy::GenerateMesh(Heightmap, LODChunkSize, LODQuadSize, Terrain->HeightThreshold, *MeshData);
		break;

	case EMeshStrategy::VertexClustering:
		VertexClusteringStrategy::GenerateMesh(Heightmap, LODChunkSize, LODQuadSize, Terrain->DecimationFactor, *MeshData);
		break;

	case EMeshStrategy::QuadraticErrorMetrics:
		QuadraticErrorMetricsStrategy::GenerateMesh(Heightmap, LODChunkSize, LODQuadSize, Terrain->QEMDecimationThreshold, *MeshData);
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid mesh optimization strategy -> using default"));
		DefaultStrategy::GenerateMesh(Heightmap, LODChunkSize, LODQuadSize, *MeshData);
		break;
	}

	LODMeshData.Add(LODLevel, MeshData);
}
#include "ProceduralTerrain/TerrainChunk.h"
#include "ProceduralTerrain/InfiniteTerrain.h"
#include "ProceduralTerrain/Enums/TerrainEnums.h"
#include "ProceduralTerrain/MeshStrategies/DefaultStrategy.h"
#include "ProceduralTerrain/MeshStrategies/QuadTreeStrategy.h"
#include "ProceduralTerrain/MeshStrategies/VertexClusteringStrategy.h"
#include "ProceduralTerrain/MeshStrategies/QuadraticErrorMetricsStrategy.h"
#include "Profiling/ProfilingMacros.h"


TerrainChunk::TerrainChunk(FIntPoint ChunkCoords, AInfiniteTerrain* InfiniteTerrain, int LODLevel)
{
	this->Coordinates = ChunkCoords;
	this->Terrain = InfiniteTerrain;
	CurrentLODLevel = LODLevel;
}

void TerrainChunk::GenerateMeshDataAsync(TFunction<void()> OnComplete)
{
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [OnComplete, this]()
		{
			TERRAIN_SCOPE_TIME(GenerateLODsAsync);

			int32 LODCount = Terrain->LODDistances.Num();
			for (int32 LOD = 0; LOD < LODCount; LOD++)
			{
				this->GenerateLODMeshData(LOD);
			}

			TWeakPtr<TerrainChunk> WeakThis = AsShared();

			// TQueue is thread-safe, we can safely add the chunk to the queue
			Terrain->ChunksQueue.Enqueue(WeakThis);

			AsyncTask(ENamedThreads::GameThread, [WeakThis, OnComplete]()
				{
					if(!WeakThis.IsValid()) return;
					OnComplete();
				});
		});
}

void TerrainChunk::UpdateLODLevel()
{
	int32 LODLevel = Terrain->CalculateLODLevel(this->Coordinates);
	if (LODLevel == CurrentLODLevel) return;

	CurrentLODLevel = LODLevel;

	Terrain->ChunksQueue.Enqueue(AsShared());
}



void TerrainChunk::GenerateLODMeshData(int32 LODLevel)
{
	TArray<float> Heightmap;
	int32 LODChunkSize;
	int32 LODQuadSize;

	Terrain->GenerateHeightmap(this->Coordinates, LODLevel, Heightmap, LODChunkSize, LODQuadSize);
	TSharedPtr<FMeshData>	LodData = MakeShared<FMeshData>();

	// Generate mesh
	switch (Terrain->MeshReductionStrategy)
	{
	case EMeshStrategy::Default:
		DefaultStrategy::GenerateMesh(Heightmap, LODChunkSize, LODQuadSize, *LodData);
		break;

	case EMeshStrategy::QuadTree:
		QuadTreeStrategy::GenerateMesh(Heightmap, LODChunkSize, LODQuadSize, Terrain->HeightThreshold, *LodData);
		break;

	case EMeshStrategy::VertexClustering:
		VertexClusteringStrategy::GenerateMesh(Heightmap, LODChunkSize, LODQuadSize, Terrain->DecimationFactor, *LodData);
		break;

	case EMeshStrategy::QuadraticErrorMetrics:
		QuadraticErrorMetricsStrategy::GenerateMesh(Heightmap, LODChunkSize, LODQuadSize, Terrain->QEMDecimationThreshold, *LodData);
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid mesh optimization strategy -> using default"));
		DefaultStrategy::GenerateMesh(Heightmap, LODChunkSize, LODQuadSize, *LodData);
		break;
	}

	MeshData.Add(LODLevel, LodData);
}
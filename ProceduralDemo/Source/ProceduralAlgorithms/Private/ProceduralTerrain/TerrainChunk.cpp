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
	this->TerrainWeakPtr = InfiniteTerrain;
	CurrentLODLevel = LODLevel;
}

TerrainChunk::~TerrainChunk()
{
	UE_LOG(LogTemp, Warning, TEXT("Destroying chunk %s"), *Coordinates.ToString());
}

void TerrainChunk::GenerateMeshDataAsync(TFunction<void()> OnComplete)
{
	TSharedPtr<TerrainChunk> SharedThis = AsShared();

	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [OnComplete, SharedThis]()
		{
			TERRAIN_SCOPE_TIME(GenerateLODsAsync);


			if (!SharedThis->TerrainWeakPtr.IsValid()) return;
			AInfiniteTerrain* Terrain = SharedThis->TerrainWeakPtr.Get();


			int32 LODCount = Terrain->LODDistances.Num();
			for (int32 LOD = 0; LOD < LODCount; LOD++)
			{
				SharedThis->GenerateLODMeshData(LOD);
			}

			TWeakPtr<TerrainChunk> WeakThis = SharedThis;
			if (!SharedThis->TerrainWeakPtr.IsValid()) return;

			// TQueue is thread-safe, we can safely add the chunk to the queue
			Terrain->ChunksQueue.Enqueue(WeakThis);

			AsyncTask(ENamedThreads::GameThread, [WeakThis, OnComplete]()
				{
					if(!WeakThis.IsValid()) return;
					OnComplete();
				});
		});
}


void TerrainChunk::UpdateChunk(int32 LODLevel)
{
	CurrentLODLevel = LODLevel;
	if (!TerrainWeakPtr.IsValid()) return;
	TerrainWeakPtr.Get()->ChunksQueue.Enqueue(AsShared());
}


void TerrainChunk::GenerateLODMeshData(int32 LODLevel)
{
	TArray<float> Heightmap;
	int32 LODChunkSize;
	int32 LODQuadSize;

	if (!TerrainWeakPtr.IsValid()) return;
	AInfiniteTerrain* Terrain = TerrainWeakPtr.Get();

	Terrain->GenerateHeightmap(this->Coordinates, LODLevel, Heightmap, LODChunkSize, LODQuadSize);
	TSharedPtr<FMeshData>	LodData = MakeShared<FMeshData>();

	// Generate mesh
	switch (Terrain->MeshReductionStrategy)
	{
	case EMeshStrategy::Default:
		DefaultStrategy::GenerateMesh(Heightmap, LODChunkSize, LODQuadSize, *LodData, Terrain->bUseSkirt, Terrain->SkirtDepth);
		break;

	case EMeshStrategy::QuadTree:
		QuadTreeStrategy::GenerateMesh(Heightmap, LODChunkSize, LODQuadSize, Terrain->HeightThreshold, *LodData);
		break;

	//case EMeshStrategy::VertexClustering:
	//	VertexClusteringStrategy::GenerateMesh(Heightmap, LODChunkSize, LODQuadSize, Terrain->DecimationFactor, *LodData);
	//	break;

	//case EMeshStrategy::QuadraticErrorMetrics:
	//	QuadraticErrorMetricsStrategy::GenerateMesh(Heightmap, LODChunkSize, LODQuadSize, Terrain->QEMDecimationThreshold, *LodData);
	//	break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid mesh optimization strategy -> using default"));
		DefaultStrategy::GenerateMesh(Heightmap, LODChunkSize, LODQuadSize, *LodData, Terrain->bUseSkirt, Terrain->SkirtDepth);
		break;
	}

	MeshData.Add(LODLevel, LodData);
}
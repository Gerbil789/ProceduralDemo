#include "ProceduralTerrain/TerrainChunkActor.h"
#include "ProceduralTerrain/InfiniteTerrain.h"
#include "ProceduralTerrain/Enums/TerrainEnums.h"
#include "ProceduralTerrain/MeshStrategies/DefaultStrategy.h"
#include "ProceduralTerrain/MeshStrategies/QuadTreeStrategy.h"
#include "ProceduralTerrain/MeshStrategies/VertexClusteringStrategy.h"
#include "ProceduralTerrain/MeshStrategies/QuadraticErrorMetricsStrategy.h"

ATerrainChunkActor::ATerrainChunkActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
	RootComponent = MeshComponent;

	// Initialize with invalid LOD level
	CurrentLODLevel = -1;
}

void ATerrainChunkActor::GenerateLODsAsync(FIntPoint ChunkCoordinates, AInfiniteTerrain* InfiniteTerrain, TFunction<void()> OnComplete)
{
	this->Terrain = InfiniteTerrain;
	this->Coordinates = ChunkCoordinates;

	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, OnComplete]()
		{
			for (int32 LOD = 0; LOD < Terrain->LODDistances.Num(); LOD++)
			{
				GenerateLODMeshData(LOD);
			}

			// TQueue is thread-safe, we can safely add the chunk to the queue
			Terrain->ChunksQueue.Enqueue(this);

			AsyncTask(ENamedThreads::GameThread, [this, OnComplete]()
				{
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

	// Set new LOD level
	MeshComponent->CreateMeshSection(0, LODMeshData[NewLODLevel]->Vertices, LODMeshData[NewLODLevel]->Triangles, LODMeshData[NewLODLevel]->Normals, LODMeshData[NewLODLevel]->UVs, TArray<FColor>(), LODMeshData[NewLODLevel]->Tangents, true);
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
	switch (Terrain->MestStrategy)
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
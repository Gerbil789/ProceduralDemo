#include "ProceduralTerrain/TerrainChunkActor.h"
#include "ProceduralTerrain/InfiniteTerrain.h"
#include "ProceduralTerrain/Enums/MeshStrategy.h"
#include "ProceduralTerrain/MeshStrategies/DefaultStrategy.h"
#include "ProceduralTerrain/MeshStrategies/QuadTreeStrategy.h"
#include "ProceduralTerrain/MeshStrategies/VertexClusteringStrategy.h"
#include "ProceduralTerrain/MeshStrategies/QuadraticErrorMetricsStrategy.h"

ATerrainChunkActor::ATerrainChunkActor()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
	RootComponent = MeshComponent;
}

void ATerrainChunkActor::GenerateMeshDataAsync(FIntPoint ChunkCoordinates, AInfiniteTerrain* InfiniteTerrain, TFunction<void()> OnComplete)
{
	this->Terrain = InfiniteTerrain;
	// background thread
	Async(EAsyncExecution::ThreadPool, [this, ChunkCoordinates, OnComplete]()
		{
			MeshData = MakeShared<FMeshData>();

			// Generate heightmap
			TArray<float> HeightMap;
			HeightMap.Init(0.0f, (Terrain->ChunkSize + 1) * (Terrain->ChunkSize + 1));
			Terrain->ApplyModifiers(ChunkCoordinates, HeightMap);

			// Generate mesh
			switch (Terrain->MestStrategy)
			{
			case EMeshStrategy::Default:
				DefaultStrategy::GenerateMesh(HeightMap, Terrain->ChunkSize, Terrain->QuadSize, *MeshData);
				break;

			case EMeshStrategy::QuadTree:
				QuadTreeStrategy::GenerateMesh(HeightMap, Terrain->ChunkSize, Terrain->QuadSize, Terrain->HeightThreshold, *MeshData);
				break;

			case EMeshStrategy::VertexClustering:
				VertexClusteringStrategy::GenerateMesh(HeightMap, Terrain->ChunkSize, Terrain->QuadSize, Terrain->DecimationFactor, *MeshData);
				break;

			case EMeshStrategy::QuadraticErrorMetrics:
				QuadraticErrorMetricsStrategy::GenerateMesh(HeightMap, Terrain->ChunkSize, Terrain->QuadSize, Terrain->QEMDecimationThreshold, *MeshData);
				break;

			default:
				UE_LOG(LogTemp, Warning, TEXT("Invalid mesh optimization strategy -> using default"));
				DefaultStrategy::GenerateMesh(HeightMap, Terrain->ChunkSize, Terrain->QuadSize, *MeshData);
				break;
			}

			// TQueue is thread-safe, we can safely add the chunk to the queue
			Terrain->ChunksQueue.Enqueue(this);

			// callback on main thread
			Async(EAsyncExecution::TaskGraphMainThread, [OnComplete]()
				{
					OnComplete();
				});
			
		});
}

void ATerrainChunkActor::SpawnMesh()
{
	if (!MeshData.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("MeshData is null"));
		return;
	}
	if (!MeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("MeshComponent is null"));
		return;
	}

	MeshComponent->CreateMeshSection(0, MeshData->Vertices, MeshData->Triangles, MeshData->Normals, MeshData->UVs, TArray<FColor>(), MeshData->Tangents, true);

	if (Terrain->TerrainMaterial)
	{
		MeshComponent->SetMaterial(0, Terrain->TerrainMaterial);
	}
}

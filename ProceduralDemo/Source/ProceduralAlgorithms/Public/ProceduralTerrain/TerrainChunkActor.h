#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralTerrain/Structs/MeshData.h"
#include "TerrainChunkActor.generated.h"

class AInfiniteTerrain; // Forward declaration

UCLASS()
class PROCEDURALALGORITHMS_API ATerrainChunkActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATerrainChunkActor();
	void GenerateLODsAsync(FIntPoint ChunkCoordinates, AInfiniteTerrain* InfiniteTerrain, TFunction<void()> OnComplete = []() {});

	void UpdateLODLevel(int32 NewLODLevel);

	// Get current LOD level
	int32 GetCurrentLODLevel() const { return CurrentLODLevel; }


	//void GenerateMeshDataAsync(FIntPoint ChunkCoordinates, AInfiniteTerrain* TerrainManager, TFunction<void()> OnComplete = []() {});

	//void SpawnMesh();
	FIntPoint Coordinates;
private:	
	AInfiniteTerrain* Terrain;
	/*TSharedPtr<FMeshData> MeshData;*/
	UProceduralMeshComponent* MeshComponent;


	TMap<int, TSharedPtr<FMeshData>> LODMeshData;

	// Current active LOD level
	int32 CurrentLODLevel;

	// Generate mesh data for a specific LOD level
	void GenerateLODMeshData(int32 LODLevel);
};

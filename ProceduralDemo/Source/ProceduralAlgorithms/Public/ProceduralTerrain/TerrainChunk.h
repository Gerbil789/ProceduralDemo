#pragma once

#include "CoreMinimal.h"
#include "ProceduralTerrain/Structs/MeshData.h"

class AInfiniteTerrain; // Forward declaration

class PROCEDURALALGORITHMS_API TerrainChunk : public TSharedFromThis<TerrainChunk>
{
public:	
	TerrainChunk(FIntPoint ChunkCoords, AInfiniteTerrain* InfiniteTerrain, int LODLevel = -1);
	~TerrainChunk();

	// Generate mesh data asynchronously and add the chunk to the queue
	void GenerateMeshDataAsync(TFunction<void()> OnComplete = []() {});

	// Update the LOD level of the chunk, if it has changed, and add the chunk to the queue
	void UpdateLODLevel(int32 LODLevel);
	int32 GetCurrentLODLevel() const { return CurrentLODLevel; }

	FIntPoint Coordinates;

	TMap<int, TSharedPtr<FMeshData>> MeshData;

private:	
	//AInfiniteTerrain* Terrain;
	TWeakObjectPtr<AInfiniteTerrain> TerrainWeakPtr;
	int32 CurrentLODLevel;

	void GenerateLODMeshData(int32 LODLevel);
};

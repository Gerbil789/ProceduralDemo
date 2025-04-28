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
	void UpdateChunk(int32 LODLevel);


	FIntPoint Coordinates;
	int32 CurrentLODLevel;
	TMap<int, TSharedPtr<FMeshData>> MeshData;

private:	
	TWeakObjectPtr<AInfiniteTerrain> TerrainWeakPtr;
	void GenerateLODMeshData(int32 LODLevel);


	TArray<TWeakPtr<TerrainChunk>> Neighbors;
};

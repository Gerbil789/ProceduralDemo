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

	void GenerateAsync(FIntPoint ChunkCoordinates, AInfiniteTerrain* TerrainManager);

private:	
	UProceduralMeshComponent* MeshComponent;

	//void GenerateMesh(TArray<float>& HeightMap, int ChunkSize, int QuadSize, FMeshData& MeshData);
	//void GenerateMeshUsingQuadTrees(TArray<float>& HeightMap, int ChunkSize, int QuadSize, float HeightTreshold, FMeshData& MeshData);
};

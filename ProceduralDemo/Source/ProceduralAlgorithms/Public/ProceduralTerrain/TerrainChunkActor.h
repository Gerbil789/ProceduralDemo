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

	void GenerateMeshDataAsync(FIntPoint ChunkCoordinates, AInfiniteTerrain* TerrainManager, TFunction<void()> OnComplete = []() {});

	void SpawnMesh();

private:	
	AInfiniteTerrain* Terrain;
	TSharedPtr<FMeshData> MeshData;
	UProceduralMeshComponent* MeshComponent;
};

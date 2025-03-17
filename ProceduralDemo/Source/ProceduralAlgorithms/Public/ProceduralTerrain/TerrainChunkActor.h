#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
//#include "InfiniteTerrain.h"
#include "TerrainChunkActor.generated.h"

class AInfiniteTerrain; // Forward declaration

UCLASS()
class PROCEDURALALGORITHMS_API ATerrainChunkActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATerrainChunkActor();

	void GenerateMesh(FIntPoint ChunkCoordinates, AInfiniteTerrain* TerrainManager);

private:	
	UProceduralMeshComponent* MeshComponent;



	
};

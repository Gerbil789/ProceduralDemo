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
	int32 GetCurrentLODLevel() const { return CurrentLODLevel; }

	FIntPoint Coordinates;

private:	
	AInfiniteTerrain* Terrain;
	TMap<int, TSharedPtr<FMeshData>> LODMeshData;
	UProceduralMeshComponent* MeshComponent;
	int32 CurrentLODLevel;

	void GenerateLODMeshData(int32 LODLevel);
};

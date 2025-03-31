#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralTerrain/Enums/TerrainEnums.h"
#include "ProceduralTerrain/Modifiers/TerrainModifier.h"
#include "InfiniteTerrain.generated.h"

class ATerrainChunkActor; // Forward declaration

// Declare the multicast delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTerrainUpdatedDelegate);



UCLASS()
class PROCEDURALALGORITHMS_API AInfiniteTerrain : public AActor
{
	GENERATED_BODY()
	
public:	
	AInfiniteTerrain();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// Expose the delegate to Blueprint
	UPROPERTY(BlueprintAssignable, Category = "Terrain")
	FOnTerrainUpdatedDelegate OnTerrainUpdated;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings")
	UMaterialInterface* TerrainMaterial;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings")
	int32 RenderRadius = 16;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings")
	int32 CleanupRadius = 32;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain | Settings", meta = (ToolTip = "How many quads in a chunk"))
	ETerrainChunkSize ChunkSize = ETerrainChunkSize::Size_16;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings", meta = (ToolTip = "Size of the quads in the chunk"))
	ETerrainQuadSize QuadSize = ETerrainQuadSize::Size_512;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings", meta = (ToolTip = "How many chunks can spawn in one frame"))
	int32 SpawnLimit = 5;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings")
	TArray<int32> LODDistances;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings")
	EMeshStrategy MestStrategy = EMeshStrategy::Default;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings", meta = (EditCondition = "MestStrategy == EMeshStrategy::QuadTree"))
	float HeightThreshold = 1.0;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings", meta = (EditCondition = "MestStrategy == EMeshStrategy::VertexClustering"))
	int DecimationFactor = 4;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings", meta = (EditCondition = "MestStrategy == EMeshStrategy::QuadraticErrorMetrics"))
	int QEMDecimationThreshold = 1000;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings")
	int PreviewRadius = 5;

	UPROPERTY(EditAnywhere, Instanced, Category = "Terrain | Modifiers")
	TArray<UTerrainModifier*> Modifiers;

	UFUNCTION(CallInEditor, meta = (ToolTip = "Preview chunks"))
	void Preview();

	UFUNCTION(CallInEditor, meta = (ToolTip = "Delete all terrain"))
	void CleanUp();

public:
	void ApplyModifiers(FIntPoint ChunkCoordinates, int32 Size, TArray<float>& HeightMap);
	void GenerateHeightmap(FIntPoint ChunkCoordinates, int32 LODLevel, TArray<float>& Heightmap, int32& LODChunkSize, int32& LODQuadSize);
	TQueue<ATerrainChunkActor*> ChunksQueue;

private:
	void InitializeTerrain();
	void UpdateChunks();
	void ClearFarChunks();

private:
	// Player tracking
	APawn* PlayerPawn = nullptr;
	FIntPoint LastChunkLocation = FIntPoint::ZeroValue;

	// Chunk management
	TMap<FIntPoint, ATerrainChunkActor*> LoadedChunks;
	FIntPoint GetChunkCoordinates(FVector2D WorldLocation);
	FVector2D GetWorldCoordinates(FIntPoint Chunk);

	TArray<FIntPoint> GetChunkCoordinatesInRadius(int Radius);

	int32 CalculateChunkLODLevel(FIntPoint ChunkCoordinates);

	float time = 0;

};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralTerrain/Enums/TerrainEnums.h"
#include "ProceduralTerrain/Modifiers/TerrainModifier.h"
#include "ProceduralMeshComponent.h"
#include "InfiniteTerrain.generated.h"

class TerrainChunk; // Forward declaration

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTerrainUpdatedDelegate);

UCLASS()
class PROCEDURALALGORITHMS_API AInfiniteTerrain : public AActor
{
	GENERATED_BODY()
	
public:	
	AInfiniteTerrain();
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Expose the delegate to Blueprint
	//UPROPERTY(BlueprintAssignable, Category = "Terrain")
	//FOnTerrainUpdatedDelegate OnTerrainUpdated;

	UPROPERTY(EditAnywhere, Instanced, Category = "Terrain | Modifiers")
	TArray<UTerrainModifier*> Modifiers;

	UFUNCTION(CallInEditor, Category = "Terrain functions", meta = (ToolTip = "Preview chunks"))
	void Preview();

	UFUNCTION(CallInEditor, Category = "Terrain functions", meta = (ToolTip = "Delete all terrain"))
	void CleanUp();

public: // Chunk actors need access to this
	void GenerateHeightmap(FIntPoint ChunkCoordinates, int32 LODLevel, TArray<float>& Heightmap, int32& LODChunkSize, int32& LODQuadSize);
	TQueue<TWeakPtr<TerrainChunk>, EQueueMode::Mpsc> ChunksQueue;
	int32 CalculateLODLevel(const FIntPoint& ChunkCoords);

protected:
	virtual void BeginPlay() override;


public:
	// === CORE SETTINGS ===

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings | Core")
	UMaterialInterface* TerrainMaterial;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings | Core")
	int32 RenderRadius = 16;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings | Core")
	int32 CleanupRadius = 32;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain | Settings | Core", meta = (ToolTip = "Number of quads in a chunk"))
	ETerrainChunkSize ChunkSize = ETerrainChunkSize::Size_16;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings | Core", meta = (ToolTip = "Size of the quads in the chunk"))
	ETerrainQuadSize QuadSize = ETerrainQuadSize::Size_512;

	// === PERFORMANCE SETTINGS ===

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings | Performance", meta = (ToolTip = "How many chunks can spawn in one frame"))
	int32 SpawnLimit = 1;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings | Performance")
	TArray<int32> LODDistances;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings | Performance")
	EMeshStrategy MeshReductionStrategy = EMeshStrategy::Default;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings | Performance", meta = (EditCondition = "MeshReductionStrategy == EMeshStrategy::QuadTree"))
	float HeightThreshold = 1.0;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings | Performance", meta = (EditCondition = "MeshReductionStrategy == EMeshStrategy::VertexClustering"))
	int DecimationFactor = 4;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings | Performance", meta = (EditCondition = "MeshReductionStrategy == EMeshStrategy::QuadraticErrorMetrics"))
	int QEMDecimationThreshold = 1000;

	// === DEBUG SETTINGS ===

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings | Debug", meta = (ToolTip = "Enable runtime generation"))
	bool bTerrainEnabled = true;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings | Debug", meta = (ToolTip = "Size of preview terrain"))
	int PreviewRadius = 5;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings | Debug")
	bool bCleanUpOnBeginPlay = false;

	UPROPERTY(EditAnywhere, Category = "Terrain | Settings | Debug", meta = (ToolTip = "Delay in miliseconds"))
	float Delay = 0.0f;

private:
	void InitializeTerrain();
	void InitializeModifiers();
	void SpawnNewChunks(const TArray<FIntPoint>& ChunkCoordinatesInRenderRadius);
	void ClearFarChunks();
	// Update LOD levels for loaded chunks
	void UpdateChunks(const TArray<FIntPoint>& ChunkCoordinatesInRenderRadius);
	void ProcessChunksQueue();

	void CreateMeshComponent(TWeakPtr<TerrainChunk> Chunk);
	void UpdateMeshComponent(TWeakPtr<TerrainChunk> Chunk);
	
	FIntPoint GetChunkCoordinates(const FVector2D& WorldLocation);
	FVector2D GetWorldCoordinates(const FIntPoint& ChunkCoords);
	TArray<FIntPoint> GetChunkCoordinatesInRadius(int32 Radius, bool Sort = false);

private:
	// Player tracking
	APawn* PlayerPawn = nullptr;
	FIntPoint LastChunkLocation = FIntPoint::ZeroValue;

	// Chunk management
	TMap<FIntPoint, TSharedPtr<TerrainChunk>> LoadedChunks;
	TMap<FIntPoint, UProceduralMeshComponent*> MeshComponents;
};

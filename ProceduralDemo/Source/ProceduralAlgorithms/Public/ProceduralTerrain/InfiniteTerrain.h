#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralTerrain/Enums/MeshStrategy.h"
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

	UPROPERTY(EditAnywhere, Category = "Terrain")
	UMaterialInterface* TerrainMaterial;

	UPROPERTY(EditAnywhere, Category = "Terrain")
	int32 RenderRadius = 15;

	UPROPERTY(EditAnywhere, Category = "Terrain")
	int32 CleanupRadius = 16;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain", meta = (ToolTip = "How many quads in a chunk"))
	int ChunkSize = 10;

	UPROPERTY(EditAnywhere, Category = "Terrain", meta = (ToolTip = "Size of the quads in the chunk"))
	int32 QuadSize = 500;

	UPROPERTY(EditAnywhere, Category = "Terrain")
	EMeshStrategy MestStrategy = EMeshStrategy::Default;

	UPROPERTY(EditAnywhere, Category = "Terrain", meta = (EditCondition = "MestStrategy == EMeshStrategy::QuadTree"))
	float HeightThreshold = 1.0;

	UPROPERTY(EditAnywhere, Category = "Terrain", meta = (EditCondition = "MestStrategy == EMeshStrategy::VertexClustering"))
	int DecimationFactor = 4;

	UPROPERTY(EditAnywhere, Category = "Terrain", meta = (EditCondition = "MestStrategy == EMeshStrategy::QuadraticErrorMetrics"))
	int QEMDecimationThreshold = 1000;

	UPROPERTY(EditAnywhere, Instanced, Category = "Terrain")
	TArray<UTerrainModifier*> Modifiers;

	UFUNCTION(CallInEditor, Category = "Terrain", meta = (ToolTip = "Preview one chunk"))
	void Preview();

	UFUNCTION(CallInEditor, Category = "Terrain", meta = (ToolTip = "Delete all terrain"))
	void CleanUp();

public:
	void UpdateChunks();
	void ClearFarChunks();
	void ApplyModifiers(FIntPoint ChunkCoordinates, TArray<float>& HeightMap);

private:
	// Player tracking
	APawn* PlayerPawn = nullptr;
	FIntPoint LastChunkLocation = FIntPoint::ZeroValue;

	// Chunk management
	TMap<FIntPoint, ATerrainChunkActor*> LoadedChunks;
	FIntPoint GetChunkCoordinates(FVector2D WorldLocation);
	FVector2D GetWorldCoordinates(FIntPoint Chunk);
};

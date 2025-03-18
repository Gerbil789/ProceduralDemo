#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralTerrain/Modifiers/TerrainModifier.h"
#include "InfiniteTerrain.generated.h"

class ATerrainChunkActor; // Forward declaration

UENUM(BlueprintType)
enum class EMeshOptimization : uint8
{
	None			UMETA(DisplayName = "None"),       
	QuadTrees UMETA(DisplayName = "QuadTrees")
};

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
	EMeshOptimization MeshOptimization = EMeshOptimization::None;

	UPROPERTY(EditAnywhere, Category = "Terrain", meta = (EditCondition = "MeshOptimization == EMeshOptimization::QuadTrees"))
	float HeightTreshold = 10;

	UPROPERTY(EditAnywhere, Instanced, Category = "Terrain")
	TArray<UTerrainModifier*> Modifiers;


	//UFUNCTION(BlueprintCallable, Category = "Terrain")
	void UpdateChunks();

	//UFUNCTION(BlueprintCallable, Category = "Terrain")
	void ClearFarChunks();

	UFUNCTION(BlueprintImplementableEvent, Category = "Terrain")
	void OnChunkGenerated(); // Notify Blueprints

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

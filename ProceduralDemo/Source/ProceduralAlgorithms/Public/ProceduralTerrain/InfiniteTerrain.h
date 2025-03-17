#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralTerrain/Modifiers/TerrainModifier.h"
#include "ProceduralTerrain.h"
#include "InfiniteTerrain.generated.h"

class ATerrainChunkActor; // Forward declaration

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

	UPROPERTY(EditAnywhere, Category = "Terrain|Chunks")
	int32 RenderRadius = 3;

	UPROPERTY(EditAnywhere, Category = "Terrain|Chunks")
	int32 CleanupRadius = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain|Chunks", meta = (ToolTip = "How many quads in a chunk"))
	int ChunkSize = 100;

	UPROPERTY(EditAnywhere, Category = "Terrain|Chunks", meta = (ToolTip = "Size of the quads in the chunk"))
	int32 QuadSize = 100;


	UFUNCTION(BlueprintCallable, Category = "Terrain|Chunks")
	void UpdateChunks();

	UFUNCTION(BlueprintCallable, Category = "Terrain|Chunks")
	void ClearFarChunks();

	UFUNCTION(BlueprintImplementableEvent, Category = "Terrain|Chunks")
	void OnChunkGenerated(); // Handle this event in Blueprints


	UPROPERTY(EditAnywhere, Instanced, Category = "Terrain|Modifiers")
	TArray<UTerrainModifier*> Modifiers;

	//float CalculateHeight(FIntPoint ChunkCoordinates, float X, float Y);

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

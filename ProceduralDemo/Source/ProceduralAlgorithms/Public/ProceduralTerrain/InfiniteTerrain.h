#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralTerrain.h"
//#include "TerrainChunkActor.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain|Chunks")
	int ChunkSize = 100;

	UPROPERTY(EditAnywhere, Category = "Terrain|Chunks")
	int32 QuadSize = 100;



	UFUNCTION(BlueprintCallable, Category = "Terrain|Chunks")
	void UpdateChunks();

	UFUNCTION(BlueprintCallable, Category = "Terrain|Chunks")
	void ClearFarChunks();

	UFUNCTION(BlueprintImplementableEvent, Category = "Terrain|Chunks")
	void OnChunkGenerated(); // Handle this event in Blueprints

	float CalculateHeight(float x, float y);
	
private:
	APawn* PlayerPawn = nullptr;
	FIntPoint LastChunkLocation = FIntPoint::ZeroValue;
	TMap<FIntPoint, ATerrainChunkActor*> LoadedChunks;


	FIntPoint GetChunkCoordinates(FVector WorldLocation);
	FVector GetWorldCoordinates(FIntPoint Chunk);
};

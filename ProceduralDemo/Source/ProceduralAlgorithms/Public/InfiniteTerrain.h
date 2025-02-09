#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralTerrain.h"
#include "InfiniteTerrain.generated.h"

UCLASS()
class PROCEDURALALGORITHMS_API AInfiniteTerrain : public AProceduralTerrain
{
	GENERATED_BODY()
	
public:	
	AInfiniteTerrain();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Chunks")
	int32 ChunkDistance = 3; // How many chunks around player



	UFUNCTION(BlueprintCallable)
	void UpdateChunks();

	UFUNCTION(BlueprintCallable)
	void ClearFarChunks();
	
private:
	FVector LastPlayerPosition;
	float UpdateDistanceThreshold = 500.0f;

	TSet<FIntPoint> LoadedChunks;
	TMap<FIntPoint, int32> LoadedSectionIndices;
	
};

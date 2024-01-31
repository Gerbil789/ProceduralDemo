#pragma once

#include "CoreMinimal.h"
#include "MarchingCubesActor.h"
#include "GameFramework/Actor.h"
#include "InfiniteWorldGenerator.generated.h"

UCLASS()
class PROCEDURALDEMO_API AInfiniteWorldGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	AInfiniteWorldGenerator();
	~AInfiniteWorldGenerator();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "MarchingCubes")
	void GenerateInitialChunks(const float& radius, const FVector& playerLocation);

	UFUNCTION(BlueprintCallable, Category = "MarchingCubes")
	void CleanUpChunks();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "MarchingCubes")
	TSubclassOf<AMarchingCubesActor> MarchingCubesActorBlueptint;

	
private:
	void UpdateWorld(const FVector& PlayerLocation);

	TMap<FVector, AMarchingCubesActor*> ChunkActors;
};

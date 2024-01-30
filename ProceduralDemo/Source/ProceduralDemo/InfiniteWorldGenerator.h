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

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "MarchingCubes")

	void GenerateInitialMeshActors(const float& radius);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "MarchingCubes")
	TSubclassOf<AMarchingCubesActor> MarchingCubesActorBlueptint;

	
private:
	void UpdateWorld(const FVector& PlayerLocation);
};

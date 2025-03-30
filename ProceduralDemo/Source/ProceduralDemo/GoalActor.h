#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GoalActor.generated.h"

UCLASS()
class PROCEDURALDEMO_API AGoalActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGoalActor();

protected:
	UPROPERTY(EditAnywhere)
	int MinDistance = 50000;

	UPROPERTY(EditAnywhere)
	int MaxDistance = 100000;

protected:
	UFUNCTION(BlueprintCallable)
	FVector GetNewLocation();

};

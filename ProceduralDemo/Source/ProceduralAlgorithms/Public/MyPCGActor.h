#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PCGComponent.h"  // Include PCG
#include "MyPCGActor.generated.h"

UCLASS()
class PROCEDURALALGORITHMS_API AMyPCGActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMyPCGActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Terrain|PCG")
	UPCGComponent* PCGComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain|PCG")
	UPCGGraphInterface* PCGGraph;

	UFUNCTION(BlueprintCallable)
	void UpdatePCG();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* VolumeBox;

private:
	APawn* PlayerPawn;
	float UpdateDistanceThreshold = 500.0f;
};

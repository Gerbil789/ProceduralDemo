// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DebugSphereActor.h"
#include "GameFramework/Actor.h"
#include "MarchingCubesActor.generated.h"


UCLASS()
class PROCEDURALDEMO_API AMarchingCubesActor : public AActor
{   
	GENERATED_BODY()
	
public:	
	AMarchingCubesActor();

	UPROPERTY(EditAnywhere, Category = "MarchingCubes")
	TSubclassOf<ADebugSphereActor> DebugSphereActorBlueprint;

	UFUNCTION(BlueprintCallable, Category = "MarchingCubes")
	void GenerateMesh(const FIntVector& size);

	UFUNCTION(BlueprintCallable, Category = "MarchingCubes")
	void CleanUpMesh();

private:
	TMap<FVector, float> Points;
	TArray<AActor*> DebugSpheres;

	void GeneratePoints(const FIntVector& size, float min = 0.0f, float max = 1.0f);
	void SpawnDebugSphere(const FVector& location, const float& value);
};

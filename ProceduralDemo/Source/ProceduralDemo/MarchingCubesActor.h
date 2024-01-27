// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarchingCubesActor.generated.h"

UCLASS()
class PROCEDURALDEMO_API AMarchingCubesActor : public AActor
{   
	GENERATED_BODY()
	
public:	
	AMarchingCubesActor();

	UPROPERTY(EditAnywhere, Category = "Mesh")
	class UStaticMesh* CustomMesh;

	UFUNCTION(BlueprintCallable, Category = "MarchingCubes")
	void GenerateMesh();

	UFUNCTION(BlueprintCallable, Category = "MarchingCubes")
	void CleanUpMesh();

private:
	UStaticMeshComponent* SphereMesh;

};

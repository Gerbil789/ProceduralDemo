// Fill out your copyright notice in the Description page of Project Settings.

#include "MarchingCubesActor.h"

// Sets default values
AMarchingCubesActor::AMarchingCubesActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set up a root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

}



void AMarchingCubesActor::GenerateMesh()
{
	UE_LOG(LogTemp, Log, TEXT("Generate started"));

	if (!CustomMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("CustomMesh is not set in the editor for %s"), *GetName());
		return;
	}
	this->SphereMesh = NewObject<UStaticMeshComponent>(this, TEXT("SphereMesh"));

	if (!this->SphereMesh) {
		UE_LOG(LogTemp, Error, TEXT("SphereMesh is null"));
		return;
	}
	this->SphereMesh->SetupAttachment(GetRootComponent());
	this->SphereMesh->RegisterComponent();
	this->SphereMesh->SetStaticMesh(CustomMesh);

	UE_LOG(LogTemp, Log, TEXT("Generate finished"));
}

void AMarchingCubesActor::CleanUpMesh()
{
	UE_LOG(LogTemp, Log, TEXT("Cleanup started"));

	if (SphereMesh)
	{
		SphereMesh->DestroyComponent();
		SphereMesh = nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("Cleanup finished"));
}



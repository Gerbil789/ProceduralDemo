#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFC/Block.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GridActor.generated.h"

UCLASS()
class PROCEDURALALGORITHMS_API AGridActor : public AActor
{
	GENERATED_BODY()

public:
	AGridActor();

	UFUNCTION(CallInEditor, Category = "WFC")
	void LoadMeshes();

	UFUNCTION(CallInEditor, Category = "WFC")
	void GenerateMesh();

	UFUNCTION(CallInEditor, Category = "WFC")
	void CleanUpMesh();

private:
	bool Render();

	UPROPERTY(EditAnywhere, Category = "WFC")
	FIntVector GridSize = FIntVector(10, 10, 1);

	UPROPERTY(EditAnywhere, Category = "WFC")
	FString MeshDirectoryPath = "/Game/TestMeshes";

	UPROPERTY(EditAnywhere, Category = "WFC")
	float Delay = 0.01f;

	UPROPERTY(EditAnywhere, Category = "WFC")
	int Offset = 200;

	UPROPERTY(EditAnywhere, Category = "WFC")
	bool bAddEmptyBlock = true;

	UPROPERTY(EditAnywhere, Category = "WFC")
	bool bAddFillBlock = true;

	UPROPERTY(EditAnywhere, Category = "WFC")
	bool Debug = false;

	UPROPERTY(EditAnywhere, Category = "WFC")
	TArray<FBlockWFC> Blocks;

	TMap<FIntVector, FBlockWFC> Grid;
	TMap<UStaticMesh*, UInstancedStaticMeshComponent*> ISMComponents = TMap<UStaticMesh*, UInstancedStaticMeshComponent*>();
};


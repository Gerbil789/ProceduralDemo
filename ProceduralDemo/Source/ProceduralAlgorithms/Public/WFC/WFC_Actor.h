#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFC_Block.h"
#include "WFC_DataSet.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "WFC_Actor.generated.h"

UCLASS()
class PROCEDURALALGORITHMS_API AWFC_Actor : public AActor
{
	GENERATED_BODY()

public:
	AWFC_Actor();
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	UFUNCTION(CallInEditor, Category = "WFC")
	void GenerateMesh();

	UFUNCTION(CallInEditor, Category = "WFC")
	void CleanUpMesh();

	bool LoadDataset();
	bool Render();

	UPROPERTY(EditAnywhere, Category = "WFC")
	UWFC_DataSet* Dataset;

	UPROPERTY(EditAnywhere, Category = "WFC")
	FIntVector GridSize = FIntVector(10, 10, 1);

	UPROPERTY(EditAnywhere, Category = "WFC")
	int Offset = 200;

	UPROPERTY(EditAnywhere, Category = "WFC")
	bool Debug = false;

	UPROPERTY(EditAnywhere, Category = "WFC")
	float Delay = 0.01f;

	TArray<FWFC_Block> Blocks;
	TMap<FIntVector, FWFC_Block> Grid;
	TMap<UStaticMesh*, UInstancedStaticMeshComponent*> ISMComponents = TMap<UStaticMesh*, UInstancedStaticMeshComponent*>();
};


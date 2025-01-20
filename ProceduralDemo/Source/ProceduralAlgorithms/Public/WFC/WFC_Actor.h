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

	UPROPERTY(EditAnywhere, Category = "WFC")
	UWFC_DataSet* Dataset = nullptr;

	UPROPERTY(EditAnywhere, Category = "WFC")
	FIntVector GridSize = FIntVector(10, 10, 1);

	UFUNCTION(CallInEditor, Category = "WFC")
	void GenerateMesh();

	bool LoadDataset();
private:
	UFUNCTION(CallInEditor, Category = "WFC")
	void CleanUpMesh();

	bool Render();

	UPROPERTY(EditAnywhere, Category = "WFC")
	int Offset = 200;

	TArray<FWFC_Block> Blocks;
	TMap<FIntVector, FWFC_Block> Grid;
	TMap<UStaticMesh*, UInstancedStaticMeshComponent*> ISMComponents = TMap<UStaticMesh*, UInstancedStaticMeshComponent*>();
};


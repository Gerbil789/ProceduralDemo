#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFC_Block.h"
#include "WFC_DataSet.h"
#include "WFC/WFC.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "UObject/UnrealType.h"
#include "WFC_Actor.generated.h"

UCLASS()
class PROCEDURALALGORITHMS_API AWFC_Actor : public AActor
{
	GENERATED_BODY()

public:
	AWFC_Actor();

protected:
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent);

private:
	UFUNCTION(CallInEditor, Category = "WFC")
	void GenerateMesh();

	UFUNCTION(CallInEditor, Category = "WFC")
	void CleanUpMesh();

	bool LoadDataset();

	bool Render();

	UPROPERTY(EditAnywhere, Category = "WFC")
	UWFC_DataSet* Dataset = nullptr;

	UPROPERTY(EditAnywhere, Category = "WFC")
	FIntVector GridSize = FIntVector(10, 10, 1);

	UPROPERTY(EditAnywhere, Category = "WFC")
	int Offset = 200;

	WaveFunctionCollapse WFC = WaveFunctionCollapse();
	TMap<FIntVector, FWFC_Block> Grid;
	TMap<UStaticMesh*, UInstancedStaticMeshComponent*> ISMComponents = TMap<UStaticMesh*, UInstancedStaticMeshComponent*>();
};


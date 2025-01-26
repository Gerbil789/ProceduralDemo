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

	UFUNCTION(BlueprintNativeEvent, CallInEditor, Category = "WFC")
	void GenerateMesh();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "WFC", meta = (ToolTip = "Execute WFC algorithm"))
	bool RunWFC();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "WFC", meta = (ToolTip = "Create mesh instances"))
	bool Render();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "WFC", meta = (ToolTip = "Delete all mesh instances & Clean Grid"))
	void CleanUpMesh();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	UWFC_DataSet* Dataset = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	FIntVector GridSize = FIntVector(10, 10, 1);

	UPROPERTY(BlueprintReadWrite, Category = "WFC")
	TMap<FIntVector, FWFC_Block> Grid;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	int Offset = 200;

	UFUNCTION(CallInEditor, Category = "WFC", meta = (DisplayName = "Reload"))
	void LoadDataset();

private:
	WaveFunctionCollapse WFC = WaveFunctionCollapse();
	TMap<UStaticMesh*, UInstancedStaticMeshComponent*> ISMComponents = TMap<UStaticMesh*, UInstancedStaticMeshComponent*>();
};
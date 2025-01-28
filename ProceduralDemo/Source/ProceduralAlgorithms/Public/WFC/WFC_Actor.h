#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFC_Block.h"
#include "WFC_DataSet.h"
#include "WFC/WaveFunctionCollapse.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "UObject/UnrealType.h"
#include "WFC_Actor.generated.h"

UCLASS()
class PROCEDURALALGORITHMS_API AWFC_Actor : public AWaveFunctionCollapse
{
	GENERATED_BODY()

public:
	AWFC_Actor();

protected:
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent); // Reload dataset when changed

	UFUNCTION(BlueprintNativeEvent, CallInEditor, Category = "WFC", meta = (DisplayName = "Generate"))
	void GenerateMesh();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "WFC", meta = (DisplayName = "Reload"))
	void LoadDataset();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "WFC", meta = (ToolTip = "Delete all mesh instances & Clean Grid"), meta = (DisplayName = "CleanUp"))
	void CleanUpMesh();

	UFUNCTION(BlueprintCallable, Category = "WFC", meta = (ToolTip = "Execute WFC algorithm"))
	bool RunWFC();

	UFUNCTION(BlueprintCallable, Category = "WFC", meta = (ToolTip = "Create mesh instances"))
	bool Render();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	UWFC_DataSet* Dataset = nullptr;

	//UPROPERTY(BlueprintReadWrite, Category = "WFC")
	//TMap<FIntVector, FWFC_Block> Grid;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	int Offset = 200;

private:
	TMap<UStaticMesh*, UInstancedStaticMeshComponent*> ISMComponents = TMap<UStaticMesh*, UInstancedStaticMeshComponent*>();
};
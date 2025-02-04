#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "WFC/WFC_Module.h"
#include "WFC/WFC_DataSet.h"
#include "WFC_AssetAction.generated.h"

UCLASS()
class PROCEDURALALGORITHMSEDITOR_API UWFC_AssetAction : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UWFC_AssetAction() = default;

	// Process DataSet
	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool ProcessStaticMeshes(const TArray<UStaticMesh*>& StaticMeshes, TArray<FWFC_Module>& OutBlocks);

	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool SaveDataSet(const FString& AssetPath, const TArray<FWFC_Module>& Blocks);


	// Prpcess one Block
	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool ProcessStaticMesh(UStaticMesh* StaticMesh, FWFC_Module& OutBlock);

	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool SaveBlock(const FString& AssetPath, const FWFC_Module& Block);


	// Generate collisions
	//UFUNCTION(BlueprintCallable, Category = "WFC")
	//bool GenerateCollisions(const TArray<UStaticMesh*>& StaticMeshes);

};

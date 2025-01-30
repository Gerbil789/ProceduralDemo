#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "WFC/WFC_Block.h"
#include "WFC_AssetAction.generated.h"

UCLASS()
class PROCEDURALALGORITHMSEDITOR_API UWFC_AssetAction : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UWFC_AssetAction() = default;

	// Process DataSet
	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool ProcessStaticMeshes(const TArray<UStaticMesh*>& StaticMeshes, bool bAddEmpty, bool bAddFill, TArray<FWFC_Block>& OutBlocks);

	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool SaveDataSet(const FString& AssetPath, const TArray<FWFC_Block>& Blocks);


	// Prpcess one Block
	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool ProcessStaticMesh(UStaticMesh* StaticMesh, FWFC_Block& OutBlock);

	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool SaveBlock(const FString& AssetPath, const FWFC_Block& Block);
};

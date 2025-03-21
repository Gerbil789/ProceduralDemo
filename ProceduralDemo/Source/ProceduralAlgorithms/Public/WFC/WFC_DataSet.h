#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WFC/WFC_Module.h"
#include "WFC_DataSet.generated.h"

UCLASS(BlueprintType, Blueprintable)
class PROCEDURALALGORITHMS_API UWFC_DataSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	TArray<FWFC_Module> Modules;
};

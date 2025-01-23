#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WFC/WFC_Block.h"
#include "WFC_DataSet.generated.h"


UCLASS()
class PROCEDURALALGORITHMS_API UWFC_DataSet : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	TArray<FWFC_Block> Blocks;
	
};

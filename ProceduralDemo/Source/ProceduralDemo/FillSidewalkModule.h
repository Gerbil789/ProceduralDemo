#pragma once

#include "CoreMinimal.h"
#include "WFCBlock.h"
#include "ModuleBase.h"
#include "FillSidewalkModule.generated.h"


UCLASS()
class PROCEDURALDEMO_API AFillSidewalkModule : public AModuleBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AWFC_Base> SidewalkBlock;

	virtual void Process(TMap<FIntVector, WFCBlock>& determinedBlocks, const TArray<WFCBlock>& blocks, const FIntVector& size) override;

private:
	WFCBlock FindBlock(const TArray<WFCBlock>& blocks, const FString& name);
};

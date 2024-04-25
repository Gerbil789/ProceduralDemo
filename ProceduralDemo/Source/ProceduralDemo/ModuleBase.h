#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFCBlock.h"
#include "ModuleBase.generated.h"

UCLASS(Abstract)
class PROCEDURALDEMO_API AModuleBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AModuleBase();
	virtual void Process(TMap<FIntVector, WFCBlock>& determinedBlocks, const TArray<WFCBlock>& blocks, const FIntVector& size);
protected:
	TArray<WFCBlock> Blocks;
	TMap<FIntVector, WFCBlock> DeterminedBlocks;
	FIntVector Size;
};

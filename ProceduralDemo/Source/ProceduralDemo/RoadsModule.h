#pragma once

#include "CoreMinimal.h"
#include "ModuleBase.h"
#include "RoadsModule.generated.h"

UCLASS(Abstract)
class PROCEDURALDEMO_API ARoadsModule : public AModuleBase
{
	GENERATED_BODY()
public:
	virtual void Process(TMap<FIntVector, WFCBlock>& determinedBlocks, const TArray<WFCBlock>& blocks, const FIntVector& size) override;
private:
	WFCBlock GetRandomBlock(const TArray<WFCBlock>& blocks);
	bool IsValid(WFCBlock block, FIntVector position);
	const TArray<FIntVector> Directions = 
	{ 
		FIntVector(1, 0, 0),
		FIntVector(-1, 0, 0),
		FIntVector(0, 1, 0),
		FIntVector(0, -1, 0)
	};
};

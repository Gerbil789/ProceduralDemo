#pragma once

#include "CoreMinimal.h"
#include <stack>
#include "ModuleBase.h"
#include "WFCBuildingModule.generated.h"

UCLASS()
class PROCEDURALDEMO_API AWFCBuildingModule : public AModuleBase
{
	GENERATED_BODY()
public:
	virtual void Process(TMap<FIntVector, WFCBlock>& determinedBlocks, const TArray<WFCBlock>& blocks, const FIntVector& size) override;
private:
	const TArray<FIntVector> Directions =
	{
		FIntVector(1, 0, 0),
		FIntVector(-1, 0, 0),
		FIntVector(0, 1, 0),
		FIntVector(0, -1, 0),
		FIntVector(0, 0, 1),
		FIntVector(0, 0, -1)
	};

	TMap<FIntVector, TArray<int>> Grid;
	std::stack<FIntVector> Wave;
	TSet<FIntVector> InWave;

	FIntVector GetLowestEntropyPosition();
	TArray<FIntVector> GetNeighbors(const FIntVector& position);
	WFCBlock PickBlock(const FIntVector& position);
	void Collapse(const FIntVector& position);
	void Propagate(const FIntVector& position);
	TSet<int> GetIncompatibleBlocks(const FIntVector& position);
};

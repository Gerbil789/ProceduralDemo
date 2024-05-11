#pragma once

#include "CoreMinimal.h"
#include <stack>
#include "ModuleBase.h"
#include "BuildingModule.generated.h"

UCLASS(Abstract)
class PROCEDURALDEMO_API ABuildingModule : public AModuleBase
{
	GENERATED_BODY()
public:
	virtual void Process() override;
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

	/*TMap<FIntVector, TArray<int>> UncollapsedGrid = TMap<FIntVector, TArray<int>>();
	std::stack<FIntVector> Wave;
	TSet<FIntVector> InWave;*/

	/*FIntVector GetLowestEntropyPosition();
	TArray<FIntVector> GetNeighbors(const FIntVector& position);
	int PickBlock(const FIntVector& position);
	void Collapse(const FIntVector& position);
	void Propagate(const FIntVector& position);
	TSet<int> GetIncompatibleBlocks(const FIntVector& position);*/
};

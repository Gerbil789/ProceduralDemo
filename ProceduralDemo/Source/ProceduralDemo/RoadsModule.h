#pragma once

#include "CoreMinimal.h"
#include "ModuleBase.h"
#include "RoadsModule.generated.h"

UCLASS(Abstract)
class PROCEDURALDEMO_API ARoadsModule : public AModuleBase
{
	GENERATED_BODY()
public:
	virtual void Process() override;
private:
	int GetRandomBlock(const TArray<int>& ids);
	bool IsValid(int id, FIntVector position);
	const TArray<FIntVector> Directions = 
	{ 
		FIntVector(1, 0, 0),
		FIntVector(-1, 0, 0),
		FIntVector(0, 1, 0),
		FIntVector(0, -1, 0)
	};
};

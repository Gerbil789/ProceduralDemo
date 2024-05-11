#pragma once

#include "CoreMinimal.h"
#include "Block.h"
#include "ModuleBase.h"
#include "FillSidewalkModule.generated.h"

UCLASS(Abstract)
class PROCEDURALDEMO_API AFillSidewalkModule : public AModuleBase
{
	GENERATED_BODY()
public:
	virtual void Process() override;
};

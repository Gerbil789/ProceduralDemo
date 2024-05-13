#pragma once

#include "CoreMinimal.h"
#include "ModuleBase.h"
#include "FillDetailsModule.generated.h"


UCLASS(Abstract)
class PROCEDURALDEMO_API AFillDetailsModule : public AModuleBase
{
	GENERATED_BODY()
public:
	virtual void Process() override;
};

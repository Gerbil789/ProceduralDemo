#pragma once

#include "WFC_Module.h"
#include "CoreMinimal.h"

class PROCEDURALALGORITHMS_API WFC_ModuleSet
{
public:
	WFC_ModuleSet() = default;
	WFC_ModuleSet(TArray<FWFC_Module> Modules);

	void CalculateEntropy();

	bool Collapse(FWFC_Module& OutModule, FRandomStream& RandomStream);

	TArray<FWFC_Module> Modules; 
	float Entropy = 0.0f;
};

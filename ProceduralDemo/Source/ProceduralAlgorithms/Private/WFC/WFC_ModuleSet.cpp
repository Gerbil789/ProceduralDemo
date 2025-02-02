#include "WFC/WFC_ModuleSet.h"

WFC_ModuleSet::WFC_ModuleSet(TArray<FWFC_Module> Modules)
{
	this->Modules = Modules;
	Entropy = 0.0f;

	CalculateEntropy();
}

void WFC_ModuleSet::CalculateEntropy()
{
	if (Modules.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("WFC_ModuleSet: CalculateEntropy: SlotModules is empty"));
		return;
	}

	int TotalWeight = 0;
	for (const FWFC_Module& Module : Modules)
	{
		TotalWeight += Module.Weight;
	}

	float NewEntropy = 0.0f;
	for (FWFC_Module& Module : Modules)
	{
		float Probability = (float)Module.Weight / TotalWeight;
		Module.Probability = Probability;
		if (Module.Probability <= 0)
		{
			UE_LOG(LogTemp, Error, TEXT("WaveFunctionCollapse: CalculateEntropy: Block probability is zero"));
			return;
		}
		float PLogP = Probability * FMath::Log2(Probability);
		NewEntropy -= PLogP;
	}

	Entropy = NewEntropy;
}


bool WFC_ModuleSet::Collapse(FWFC_Module& OutModule, FRandomStream& RandomStream)
{
	if (Modules.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("WFC_ModuleSet: CollapseModule: SlotModules is empty"));
		return false;
	}

	// Select random module based on probability
	float RandomValue = RandomStream.FRandRange(0.0f, 1.0f);
	float AccumulatedProbability = 0.0f;
	FWFC_Module ChosenModule;
	for (FWFC_Module& Module : Modules)
	{
		AccumulatedProbability += Module.Probability;
		if (RandomValue <= AccumulatedProbability)
		{
			ChosenModule = Module;
			break;
		}
	}

	// Collapse the slot
	Modules = { ChosenModule };
	Entropy = 0;


	OutModule = ChosenModule;
	return true;
}

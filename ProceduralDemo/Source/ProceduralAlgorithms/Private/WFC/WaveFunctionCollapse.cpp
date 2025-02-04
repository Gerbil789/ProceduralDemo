#include "WFC/WaveFunctionCollapse.h"

const TArray<FIntVector> AWaveFunctionCollapse::Directions = {
		FIntVector(1, 0, 0),
		FIntVector(-1, 0, 0),
		FIntVector(0, 1, 0),
		FIntVector(0, -1, 0),
		FIntVector(0, 0, 1),
		FIntVector(0, 0, -1)
};

void AWaveFunctionCollapse::SetModules(const TArray<FWFC_Module>& _Modules)
{
	this->Modules = _Modules;
	BuildCompatibilityTable();
}

bool AWaveFunctionCollapse::Run()
{
	if(!Initialize())
	{
		UE_LOG(LogTemp, Error, TEXT("WaveFunctionCollapse: Failed to initialize"));
		return false;
	}

	while (true)
	{
		FIntVector LowestEntropyCell;
		if (!FindLowestEntropySlot(LowestEntropyCell))
		{
			break; // We have collapsed all cells
		}

		if (!CollapseSlot(LowestEntropyCell))
		{
			break; // No valid modules to collapse to
		}

		Propagate(LowestEntropyCell);
	}

	return true;
}

bool AWaveFunctionCollapse::Initialize()
{
	if(Modules.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("WaveFunctionCollapse: No modules to initialize"));
		return false;
	}

	int TotalSlots = GridSize.X * GridSize.Y * GridSize.Z;
	Wave.SetNum(TotalSlots);
	Grid.Reserve(TotalSlots);


	if (RandomSeed)
	{
		Seed = FMath::Rand();
	}

	RandomStream.Initialize(Seed);

	for (int i = 0; i < TotalSlots; i++)
	{
		Wave[i] = WFC_ModuleSet(Modules);
	}

	return true;
}

bool AWaveFunctionCollapse::CollapseSlot(const FIntVector& Position)
{
	int Index = GetIndex(Position);
	FWFC_Module CollapsedModule;
	
	if(!Wave[Index].Collapse(CollapsedModule, RandomStream))
	{
		UE_LOG(LogTemp, Error, TEXT("WaveFunctionCollapse: Failed to collapse slot (%d, %d, %d)"), Position.X, Position.Y, Position.Z);
		return false;
	}

	// Update the grid and wave
	Grid.Add(Position, CollapsedModule);
	return true;
}

bool AWaveFunctionCollapse::CollapseSlotToModule(const FIntVector& Position, const FWFC_Module& Module)
{
	int Index = GetIndex(Position);
	if (Wave[Index].Modules.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("No modules to collapse to at slot (%d, %d, %d)"), Position.X, Position.Y, Position.Z);
		return false;
	}

	if (!Wave[Index].Modules.Contains(Module))
	{
		UE_LOG(LogTemp, Error, TEXT("Module is not compatible with slot (%d, %d, %d)"), Position.X, Position.Y, Position.Z);
		return false;
	}

	Wave[Index].Modules = { Module };
	Wave[Index].Entropy = 0;
	Grid.Add(Position, Module);
	return true;
}

void AWaveFunctionCollapse::Propagate(const FIntVector& Position)
{
	TArray<FIntVector> PropagationStack;
	PropagationStack.Add(Position);

	while (!PropagationStack.IsEmpty())
	{
		FIntVector CurrentPosition = PropagationStack.Pop();
		int CurrentIndex = GetIndex(CurrentPosition);

		for (const FIntVector& Dir : Directions)
		{
			FIntVector NeighborPosition = CurrentPosition + Dir;

			if (NeighborPosition.X < 0 || NeighborPosition.X >= GridSize.X ||
				NeighborPosition.Y < 0 || NeighborPosition.Y >= GridSize.Y ||
				NeighborPosition.Z < 0 || NeighborPosition.Z >= GridSize.Z)
			{
				continue; // Out of bounds
			}

			int NeighborIndex = GetIndex(NeighborPosition);
			TArray<FWFC_Module>& NeighborModules = Wave[NeighborIndex].Modules;
			TArray<FWFC_Module> ValidModules;

			TArray<FWFC_Module> CurrentModules = Wave[CurrentIndex].Modules;
			TSet<FWFC_Module> ValidNeighborsSet;
			for (const FWFC_Module& CurrentModule : CurrentModules)
			{
				if (CompatibilityTable[Dir].Contains(CurrentModule))
				{
					ValidNeighborsSet.Append(CompatibilityTable[Dir][CurrentModule]);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("WaveFunctionCollapse: CompatibilityTable does not contain module"));
				}
			}

			// Convert the set to an array
			TArray<FWFC_Module> ValidNeighbors = ValidNeighborsSet.Array();

			for (const FWFC_Module& NeighborModule : NeighborModules)
			{
				if (ValidNeighbors.Contains(NeighborModule))
				{
					ValidModules.Add(NeighborModule);
				}
			}


			if (ValidModules.Num() < NeighborModules.Num())
			{
				Wave[NeighborIndex].Modules = ValidModules;
				Wave[NeighborIndex].CalculateEntropy();

				if (ValidModules.Num() == 1)
				{
					Grid.Add(NeighborPosition, ValidModules[0]);
				}

				PropagationStack.Add(NeighborPosition);
			}
		}
	}
}

int AWaveFunctionCollapse::GetIndex(const FIntVector& Position) const
{
	return Position.X + Position.Y * GridSize.X + Position.Z * GridSize.X * GridSize.Y;
}

void AWaveFunctionCollapse::BuildCompatibilityTable()
{
	CompatibilityTable.Empty();
	CompatibilityTable.Reserve(6);

	for (const FIntVector& Dir : Directions)
	{
		TMap<FWFC_Module, TArray<FWFC_Module>>& DirectionTable = CompatibilityTable.Add(Dir);

		for (const FWFC_Module& ModuleA : Modules)
		{
			TArray<FWFC_Module> ValidNeighbors;

			for (const FWFC_Module& ModuleB : Modules)
			{
				if(ModuleA.IsCompatibleWith(ModuleB, Dir))
				{
					ValidNeighbors.Add(ModuleB);
				}
			}
			DirectionTable.Add(ModuleA, ValidNeighbors);
		}
	}
}

bool AWaveFunctionCollapse::FindLowestEntropySlot(FIntVector& OutPosition)
{
	int MinEntropy = TNumericLimits<int>::Max();
	TArray<FIntVector> LowestEntropySlots; // Store all cells with the minimum entropy

	// Find the minimum entropy and collect all cells with that entropy
	for (int i = 0; i < Wave.Num(); ++i)
	{
		const auto& Slot = Wave[i];
		if (Slot.Entropy == 0) continue; // Skip collapsed cells

		FIntVector Position(i % GridSize.X, (i / GridSize.X) % GridSize.Y, i / (GridSize.X * GridSize.Y));

		if (Slot.Entropy < MinEntropy)
		{
			MinEntropy = Slot.Entropy;
			LowestEntropySlots = { Position }; // Reset and add new min entropy slot
		}
		else if (Slot.Entropy == MinEntropy)
		{
			LowestEntropySlots.Add(Position);
		}
	}

	if (LowestEntropySlots.IsEmpty()) return false;

	if (LowestEntropySlots.Num() == 1)
	{
		OutPosition = LowestEntropySlots[0];
		return true;
	}

	OutPosition = LowestEntropySlots[RandomStream.RandRange(0, LowestEntropySlots.Num() - 1)];

	return true;
}

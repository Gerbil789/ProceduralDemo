#include "WFC/WaveFunctionCollapse.h"

AWaveFunctionCollapse::AWaveFunctionCollapse()
{

}

void AWaveFunctionCollapse::SetBlocks(const TArray<FWFC_Block>& _Blocks)
{
	this->Blocks = _Blocks;
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
		if (!FindLowestEntropyCell(LowestEntropyCell))
		{
			break; // We have collapsed all cells
		}

		if (!CollapseCell(LowestEntropyCell))
		{
			break; // No valid blocks to collapse to
		}

		Propagate(LowestEntropyCell);
	}

	return true;
}

bool AWaveFunctionCollapse::Initialize()
{
	if(Blocks.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("WaveFunctionCollapse: No blocks to initialize"));
		return false;
	}

	int TotalCells = GridSize.X * GridSize.Y * GridSize.Z;
	Wave.SetNum(TotalCells);
	Entropies.SetNum(TotalCells);
	Grid.Reserve(TotalCells);


	if (!Grid.IsEmpty())
	{
		//update compatibility table
		int newBlocks = 0;
		for (auto& Pair : Grid)
		{
			FIntVector Position = Pair.Key;
			FWFC_Block Block = Pair.Value;

			if (!Blocks.Contains(Block))
			{
				Blocks.Add(Block);
				newBlocks++;
			}

		}

		if (newBlocks > 0)
		{
			BuildCompatibilityTable();
		}
	}


	for (int i = 0; i < TotalCells; ++i)
	{
		Wave[i] = Blocks;
		FIntVector Position = FIntVector(i % GridSize.X, (i / GridSize.X) % GridSize.Y, i / (GridSize.X * GridSize.Y));
		Entropies[i] = CalculateEntropy(Position);
	}

	//handle already collapsed cells
	for (const auto& Pair : Grid)
	{
		FIntVector Position = Pair.Key;
		FWFC_Block Block = Pair.Value;
		int Index = GetIndex(Position);
		Wave[Index] = { Block };
		Entropies[Index] = 0;
	}

	return true;
}

bool AWaveFunctionCollapse::CollapseCell(const FIntVector& Position)
{
	int Index = GetIndex(Position);
	TArray<FWFC_Block>& CellBlocks = Wave[Index];

	if (CellBlocks.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("No blocks to collapse to at cell (%d, %d, %d)"), Position.X, Position.Y, Position.Z);
		return false;
	}

	// Calculate the total weight (sum of priorities)
	float TotalWeight = 0.0f;
	TArray<float> CumulativeWeights; // Stores cumulative weights for weighted random selection

	for (const FWFC_Block& Block : CellBlocks)
	{
		TotalWeight += FMath::Max(0, Block.Priority); // Ensure no negative weights
		CumulativeWeights.Add(TotalWeight);
	}

	if (TotalWeight <= 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("All blocks have zero or negative priority at cell (%d, %d, %d)"), Position.X, Position.Y, Position.Z);
		return false;
	}

	// Generate a random number between 0 and TotalWeight
	float RandomValue = FMath::FRandRange(0.0f, TotalWeight);

	// Find the block corresponding to the random value
	int ChosenIndex = 0;
	for (int i = 0; i < CumulativeWeights.Num(); ++i)
	{
		if (RandomValue <= CumulativeWeights[i])
		{
			ChosenIndex = i;
			break;
		}
	}

	// Select the block
	FWFC_Block ChosenBlock = CellBlocks[ChosenIndex];

	// Update the grid and wave
	Grid.Add(Position, ChosenBlock);
	CellBlocks = { ChosenBlock }; // Reduce the possibilities to just the chosen block
	Entropies[Index] = 0;


	return true;
}

bool AWaveFunctionCollapse::CollapseCellWithBlock(const FIntVector& Position, const FWFC_Block& Block)
{
	int Index = GetIndex(Position);
	if (Wave[Index].IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("No blocks to collapse to at cell (%d, %d, %d)"), Position.X, Position.Y, Position.Z);
		return false;
	}

	if (!Wave[Index].Contains(Block))
	{
		UE_LOG(LogTemp, Error, TEXT("Block is not compatible with cell (%d, %d, %d)"), Position.X, Position.Y, Position.Z);
		return false;
	}

	Wave[Index] = { Block };
	Entropies[Index] = 0;	
	Grid.Add(Position, Block);
	return true;
}

void AWaveFunctionCollapse::Propagate(const FIntVector& Position)
{
	const TArray<FIntVector> Directions =
	{
			FIntVector(1, 0, 0),
			FIntVector(-1, 0, 0),
			FIntVector(0, 1, 0),
			FIntVector(0, -1, 0),
			FIntVector(0, 0, 1),
			FIntVector(0, 0, -1)
	};


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
			TArray<FWFC_Block>& NeighborBlocks = Wave[NeighborIndex];
			TArray<FWFC_Block> ValidBlocks;

			TArray<FWFC_Block> CurrentBlocks = Wave[CurrentIndex];
			TSet<FWFC_Block> ValidNeighborsSet;
			for (const FWFC_Block& CurrentBlock : CurrentBlocks)
			{
				if (CompatibilityTable[Dir].Contains(CurrentBlock))
				{
					ValidNeighborsSet.Append(CompatibilityTable[Dir][CurrentBlock]);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("WaveFunctionCollapse: CompatibilityTable does not contain block"));
				}
			}

			// Convert the set to an array
			TArray<FWFC_Block> ValidNeighbors = ValidNeighborsSet.Array();

			for (const FWFC_Block& NeighborBlock : NeighborBlocks)
			{
				if (ValidNeighbors.Contains(NeighborBlock))
				{
					ValidBlocks.Add(NeighborBlock);
				}
			}


			if (ValidBlocks.Num() < NeighborBlocks.Num())
			{
				//if (ValidBlocks.IsEmpty())
				//{
				//	ErrorMsg = FString::Printf(TEXT("No valid blocks to propagate to at cell (%d, %d, %d)"), NeighborPosition.X, NeighborPosition.Y, NeighborPosition.Z);
				//	return;
				//}


				Wave[NeighborIndex] = ValidBlocks;
				Entropies[NeighborIndex] = CalculateEntropy(NeighborPosition);

				if (ValidBlocks.Num() == 1)
				{
					Grid.Add(NeighborPosition, ValidBlocks[0]);
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

int AWaveFunctionCollapse::CalculateEntropy(const FIntVector& Position) const
{
	int Index = GetIndex(Position);
	const TArray<FWFC_Block>& SlotModules = Wave[Index];

	if (SlotModules.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("WaveFunctionCollapse: CalculateEntropy: SlotModules is empty"));
		return 0;
	}

	int Entropy = 0;
	for (const FWFC_Block& Block : SlotModules)
	{
		Entropy += Block.Priority;
	}
	return Entropy;
}

void AWaveFunctionCollapse::BuildCompatibilityTable()
{
	CompatibilityTable.Empty();
	CompatibilityTable.Reserve(6);

	const TArray<FIntVector> Directions =
	{
			FIntVector(1, 0, 0),
			FIntVector(-1, 0, 0),
			FIntVector(0, 1, 0),
			FIntVector(0, -1, 0),
			FIntVector(0, 0, 1),
			FIntVector(0, 0, -1)
	};

	// Precompute valid neighbors for each block and direction
	for (const FIntVector& Dir : Directions)
	{
		TMap<FWFC_Block, TArray<FWFC_Block>>& DirectionTable = CompatibilityTable.Add(Dir);

		for (const FWFC_Block& BlockA : Blocks)
		{
			TArray<FWFC_Block> ValidNeighbors;

			for (const FWFC_Block& BlockB : Blocks)
			{
				if (CheckCompatibility(BlockA, BlockB, Dir))
				{
					ValidNeighbors.Add(BlockB);
				}
			}

			DirectionTable.Add(BlockA, ValidNeighbors);
		}
	}


}

bool AWaveFunctionCollapse::CheckCompatibility(const FWFC_Block& CollapsedBlock, const FWFC_Block& NeighborBlock, const FIntVector& Direction)
{
	if (Direction == FIntVector(1, 0, 0))
	{
		return CollapsedBlock.SocketFront.CanConnect(NeighborBlock.SocketBack);
	}
	else if (Direction == FIntVector(-1, 0, 0))
	{
		return CollapsedBlock.SocketBack.CanConnect(NeighborBlock.SocketFront);
	}
	else if (Direction == FIntVector(0, 1, 0))
	{
		return CollapsedBlock.SocketRight.CanConnect(NeighborBlock.SocketLeft);
	}
	else if (Direction == FIntVector(0, -1, 0))
	{
		return CollapsedBlock.SocketLeft.CanConnect(NeighborBlock.SocketRight);
	}
	else if (Direction == FIntVector(0, 0, 1))
	{
		return CollapsedBlock.SocketUp.CanConnect(NeighborBlock.SocketDown);
	}
	else if (Direction == FIntVector(0, 0, -1))
	{
		return CollapsedBlock.SocketDown.CanConnect(NeighborBlock.SocketUp);
	}

	UE_LOG(LogTemp, Error, TEXT("CheckCompatibility: Invalid direction"));
	return false;
}

bool AWaveFunctionCollapse::FindLowestEntropyCell(FIntVector& OutPosition)
{
	int MinEntropy = TNumericLimits<int>::Max();
	TArray<FIntVector> LowestEntropySlots; // Store all cells with the minimum entropy

	// Find the minimum entropy and collect all cells with that entropy
	for (int i = 0; i < Entropies.Num(); ++i)
	{
		// Entropy == 1 means the cell is already collapsed
		if (Entropies[i] > 0)
		{
			if (Entropies[i] < MinEntropy)
			{
				// Found a new minimum entropy
				MinEntropy = Entropies[i];
				LowestEntropySlots.Reset(); // Clear the previous list
				LowestEntropySlots.Add(FIntVector(i % GridSize.X, (i / GridSize.X) % GridSize.Y, i / (GridSize.X * GridSize.Y)));
			}
			else if (Entropies[i] == MinEntropy)
			{
				// Found another cell with the same minimum entropy
				LowestEntropySlots.Add(FIntVector(i % GridSize.X, (i / GridSize.X) % GridSize.Y, i / (GridSize.X * GridSize.Y)));
			}
		}
	}

	// If no cell has entropy > 1, we are done
	if (LowestEntropySlots.IsEmpty())
	{
		return false;
	}

	if (LowestEntropySlots.Num() == 1)
	{
		OutPosition = LowestEntropySlots[0];
		return true;
	}


	// Select module from the lowest entropy slots where the module has the least amount of possibilities
	int MinPossibilities = TNumericLimits<int>::Max();
	FIntVector MinPossibilitiesPosition = FIntVector(-1, -1, -1);

	for (const FIntVector& Position : LowestEntropySlots)
	{
		int Index = GetIndex(Position);
		int NumPossibilities = Wave[Index].Num();
		if (NumPossibilities < MinPossibilities)
		{
			MinPossibilities = NumPossibilities;
			MinPossibilitiesPosition = Position;
		}
	}

	if (MinPossibilitiesPosition == FIntVector(-1, -1, -1))
	{
		UE_LOG(LogTemp, Error, TEXT("WaveFunctionCollapse: FindLowestEntropyCell: MinPossibilitiesPosition is invalid"));
		return false;
	}

	OutPosition = MinPossibilitiesPosition;


	// //Sort cells by Z coordinate (ascending order)
	//Algo::Sort(LowestEntropySlots, [](const FIntVector& A, const FIntVector& B)
	//	{
	//		return A.Z < B.Z; // Sort by Z coordinate
	//	});

	//// Find all cells with the lowest Z coordinate
	//int LowestZ = LowestEntropySlots[0].Z; // The lowest Z coordinate after sorting
	//TArray<FIntVector> LowestZCells;

	//for (const FIntVector& Cell : LowestEntropySlots)
	//{
	//	if (Cell.Z == LowestZ)
	//	{
	//		LowestZCells.Add(Cell);
	//	}
	//	else
	//	{
	//		break; // All remaining cells have higher Z coordinates
	//	}
	//}

	//// Randomly select one of the cells with the lowest Z coordinate
	//int RandomIndex = FMath::RandRange(0, LowestZCells.Num() - 1);
	//OutPosition = LowestZCells[RandomIndex];

	//OutPosition = LowestEntropyCells[FMath::RandRange(0, LowestEntropyCells.Num() - 1)];

	return true;
}

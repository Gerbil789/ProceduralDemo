#include "WFC/WFC.h"

void WaveFunctionCollapse::SetBlocks(const TArray<FWFC_Block>& _Blocks)
{
	this->Blocks = _Blocks;
	BuildCompatibilityTable();
}

bool WaveFunctionCollapse::Run(TMap<FIntVector, FWFC_Block>& OutGrid, const FIntVector& Size)
{
	if(!Initialize(OutGrid, Size))
	{
		UE_LOG(LogTemp, Error, TEXT("WaveFunctionCollapse: Failed to initialize"));
		return false;
	}

	while (true)
	{
		// Find the lowest-entropy cell
		FIntVector LowestEntropyCell;
		if (!FindLowestEntropyCell(LowestEntropyCell))
		{
			break; // We have collapsed all cells
		}

		// Collapse the lowest-entropy cell
		if (!CollapseCell(LowestEntropyCell, OutGrid))
		{
			break;
		}

		// Propagate the changes
		Propagate(LowestEntropyCell, OutGrid);
	}

	if (!ErrorMsg.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("WaveFunctionCollapse: %s"), *ErrorMsg);
		return false;
	}

	return true;
}

bool WaveFunctionCollapse::Initialize(TMap<FIntVector, FWFC_Block>& OutGrid, const FIntVector& Size)
{
	if(Blocks.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("WaveFunctionCollapse: No blocks to initialize"));
		return false;
	}

	ErrorMsg = FString();
	this->GridSize = Size;
	int TotalCells = GridSize.X * GridSize.Y * GridSize.Z;
	Wave.SetNum(TotalCells);
	Entropies.SetNum(TotalCells);
	OutGrid.Reserve(TotalCells);


	if (!OutGrid.IsEmpty())
	{
		//update compatibility table
		int newBlocks = 0;
		for (auto& Pair : OutGrid)
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
		Entropies[i] = Blocks.Num();
	}

	//handle already collapsed cells
	TMap<FIntVector, FWFC_Block> ToPropagate;
	for (const auto& Pair : OutGrid)
	{
		FIntVector Position = Pair.Key;
		FWFC_Block Block = Pair.Value;
		int Index = GetIndex(Position);
		Wave[Index] = { Block };
		Entropies[Index] = 1;
		ToPropagate.Add(Position, Block);
	}

	//propagate collapsed cells
	for (const auto& Pair : ToPropagate)
	{
		FIntVector Position = Pair.Key;
		Propagate(Position, OutGrid);
	}

	return true;
}

bool WaveFunctionCollapse::CollapseCell(const FIntVector& Position, TMap<FIntVector, FWFC_Block>& OutGrid)
{
	int Index = GetIndex(Position);
	TArray<FWFC_Block>& CellBlocks = Wave[Index];

	if (CellBlocks.IsEmpty())
	{
		ErrorMsg = FString::Printf(TEXT("No blocks to collapse to at cell (%d, %d, %d)"), Position.X, Position.Y, Position.Z);
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
		ErrorMsg = FString::Printf(TEXT("All blocks have zero or negative priority at cell (%d, %d, %d)"), Position.X, Position.Y, Position.Z);
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
	OutGrid.Add(Position, ChosenBlock);
	CellBlocks = { ChosenBlock }; // Reduce the possibilities to just the chosen block
	Entropies[Index] = 1;


	return true;
}

void WaveFunctionCollapse::Propagate(const FIntVector& Position, TMap<FIntVector, FWFC_Block>& OutGrid)
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
				Wave[NeighborIndex] = ValidBlocks;
				Entropies[NeighborIndex] = ValidBlocks.Num();

				if (ValidBlocks.Num() == 1)
				{
					OutGrid.Add(NeighborPosition, ValidBlocks[0]);
				}

				PropagationStack.Add(NeighborPosition);
			}
		}
	}
}

int WaveFunctionCollapse::GetIndex(const FIntVector& Position) const
{
	return Position.X + Position.Y * GridSize.X + Position.Z * GridSize.X * GridSize.Y;
}

void WaveFunctionCollapse::BuildCompatibilityTable()
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

bool WaveFunctionCollapse::CheckCompatibility(const FWFC_Block& CollapsedBlock, const FWFC_Block& NeighborBlock, const FIntVector& Direction)
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

bool WaveFunctionCollapse::FindLowestEntropyCell(FIntVector& OutPosition) const
{
	int MinEntropy = TNumericLimits<int>::Max();
	TArray<FIntVector> LowestEntropyCells; // Store all cells with the minimum entropy

	// Find the minimum entropy and collect all cells with that entropy
	for (int i = 0; i < Entropies.Num(); ++i)
	{
		// Entropy == 1 means the cell is already collapsed
		if (Entropies[i] > 1)
		{
			if (Entropies[i] < MinEntropy)
			{
				// Found a new minimum entropy
				MinEntropy = Entropies[i];
				LowestEntropyCells.Reset(); // Clear the previous list
				LowestEntropyCells.Add(FIntVector(i % GridSize.X, (i / GridSize.X) % GridSize.Y, i / (GridSize.X * GridSize.Y)));
			}
			else if (Entropies[i] == MinEntropy)
			{
				// Found another cell with the same minimum entropy
				LowestEntropyCells.Add(FIntVector(i % GridSize.X, (i / GridSize.X) % GridSize.Y, i / (GridSize.X * GridSize.Y)));
			}
		}
	}

	// If no cell has entropy > 1, we are done
	if (LowestEntropyCells.IsEmpty())
	{
		return false;
	}

	// //Sort cells by Z coordinate (ascending order)
	Algo::Sort(LowestEntropyCells, [](const FIntVector& A, const FIntVector& B)
		{
			return A.Z < B.Z; // Sort by Z coordinate
		});

	// Find all cells with the lowest Z coordinate
	int LowestZ = LowestEntropyCells[0].Z; // The lowest Z coordinate after sorting
	TArray<FIntVector> LowestZCells;

	for (const FIntVector& Cell : LowestEntropyCells)
	{
		if (Cell.Z == LowestZ)
		{
			LowestZCells.Add(Cell);
		}
		else
		{
			break; // All remaining cells have higher Z coordinates
		}
	}

	// Randomly select one of the cells with the lowest Z coordinate
	int RandomIndex = FMath::RandRange(0, LowestZCells.Num() - 1);
	OutPosition = LowestZCells[RandomIndex];

	//OutPosition = LowestEntropyCells[FMath::RandRange(0, LowestEntropyCells.Num() - 1)];

	return true;
}

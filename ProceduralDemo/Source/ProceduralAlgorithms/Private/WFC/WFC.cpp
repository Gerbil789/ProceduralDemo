#include "WFC/WFC.h"

bool WaveFunctionCollapse::Run(const TArray<FWFC_Block>& Blocks, TMap<FIntVector, FWFC_Block>& OutGrid, const FIntVector& Size)
{
	Initialize(Blocks, Size, OutGrid);

	while (true)
	{
		// Find the lowest-entropy cell
		FIntVector LowestEntropyCell;
		if (!FindFowestEntropyCell(LowestEntropyCell))
		{
			break; // We have collapsed all cells -> done
		}

		// Collapse the lowest-entropy cell
		if (!CollapseCell(LowestEntropyCell, OutGrid))
		{
			UE_LOG(LogTemp, Error, TEXT("WaveFunctionCollapse: Failed to collapse cell"));
			return false;
		}

		// Propagate the changes
		Propagate(LowestEntropyCell, OutGrid);
	}

	return true;
}

void WaveFunctionCollapse::Initialize(const TArray<FWFC_Block>& Blocks, const FIntVector& Size, TMap<FIntVector, FWFC_Block>& OutGrid)
{
	this->GridSize = Size;
	int TotalCells = GridSize.X * GridSize.Y * GridSize.Z;
	Wave.SetNum(TotalCells);
	Entropies.SetNum(TotalCells);
	OutGrid.Reserve(TotalCells);

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
}

bool WaveFunctionCollapse::CollapseCell(const FIntVector& Position, TMap<FIntVector, FWFC_Block>& OutGrid)
{
	int Index = GetIndex(Position);
	TArray<FWFC_Block>& CellBlocks = Wave[Index];

	if (CellBlocks.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("CollapseCell: No blocks to collapse to"));
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
		UE_LOG(LogTemp, Error, TEXT("CollapseCell: All blocks have zero or negative priority"));
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

	while (PropagationStack.Num() > 0)
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

			for (const FWFC_Block& NeighborBlock : NeighborBlocks)
			{
				bool bIsCompatible = false;

				for (const FWFC_Block& CollapsedBlock : Wave[CurrentIndex])
				{
					if (CheckCompatibility(CollapsedBlock, NeighborBlock, Dir))
					{
						bIsCompatible = true;
						break;
					}
				}

				if (bIsCompatible)
					ValidBlocks.Add(NeighborBlock);
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
bool WaveFunctionCollapse::FindFowestEntropyCell(FIntVector& OutPosition) const
{
	int MinEntropy = TNumericLimits<int>::Max();
	FIntVector LowestEntropyCell = FIntVector(-1, -1, -1);

	for (int i = 0; i < Entropies.Num(); ++i)
	{
		// Entropy == 1 means the cell is already collapsed
		if (Entropies[i] > 1 && Entropies[i] < MinEntropy)
		{
			MinEntropy = Entropies[i];
			LowestEntropyCell = FIntVector(i % GridSize.X, (i / GridSize.X) % GridSize.Y, i / (GridSize.X * GridSize.Y));
		}
	}

	// If no cell has entropy > 1, we are done
	if (LowestEntropyCell == FIntVector(-1, -1, -1))
	{
		UE_LOG(LogTemp, Display, TEXT("WaveFunctionCollapse: No cell found"));
		return false;
	}

	OutPosition = LowestEntropyCell;
	return true;
}

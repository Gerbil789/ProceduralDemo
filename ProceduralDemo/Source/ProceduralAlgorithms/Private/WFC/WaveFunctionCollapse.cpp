#include "WFC/WaveFunctionCollapse.h"

namespace WFC
{
  bool WaveFunctionCollapse::Run(const TArray<FBlockWFC>& Blocks, TMap<FIntVector, FBlockWFC>& OutGrid, const FIntVector& Size)
  {
    Initialize(Blocks, Size);

    TArray<FIntVector> PropagationStack;

    while (true)
    {
			// Find the lowest-entropy cell
      FIntVector LowestEntropyCell;
			if(!FindFowestEntropyCell(LowestEntropyCell))
      {
        UE_LOG(LogTemp, Display, TEXT("WaveFunctionCollapse: Finished"));
        break;
      }

      // Collapse the lowest-entropy cell
      if (!CollapseCell(LowestEntropyCell, OutGrid))
      {
				UE_LOG(LogTemp, Error, TEXT("WaveFunctionCollapse: Failed to collapse cell"));
        return false;
      }

      // Propagate changes
      PropagationStack.Add(LowestEntropyCell);
      Propagate(PropagationStack, OutGrid);
    }

    return true;
  }

  void WaveFunctionCollapse::Initialize(const TArray<FBlockWFC>& Blocks, const FIntVector& Size)
  {
    this->GridSize = Size;
    int TotalCells = GridSize.X * GridSize.Y * GridSize.Z;
    Wave.SetNum(TotalCells);
    Entropies.SetNum(TotalCells);

    for (int i = 0; i < TotalCells; ++i)
    {
      Wave[i] = Blocks; // Initialize all cells with all possible blocks
      Entropies[i] = Blocks.Num();
    }
  }

  bool WaveFunctionCollapse::CollapseCell(const FIntVector& Position, TMap<FIntVector, FBlockWFC>& OutGrid)
  {
    int Index = GetIndex(Position);
    TArray<FBlockWFC>& CellBlocks = Wave[Index];

    if (CellBlocks.Num() == 0)
    {
			UE_LOG(LogTemp, Error, TEXT("CollapseCell: No blocks to collapse to"));
			return false;
    }

    // Randomly pick one block to collapse to
    int RandomIndex = FMath::RandRange(0, CellBlocks.Num() - 1);
    FBlockWFC ChosenBlock = CellBlocks[RandomIndex];

    OutGrid.Add(Position, ChosenBlock);
    CellBlocks = { ChosenBlock }; // Reduce the possibilities to just the chosen block
    Entropies[Index] = 1;

    return true;
  }

  void WaveFunctionCollapse::Propagate(TArray<FIntVector>& PropagationStack, TMap<FIntVector, FBlockWFC>& OutGrid)
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
        TArray<FBlockWFC>& NeighborBlocks = Wave[NeighborIndex];
        TArray<FBlockWFC> ValidBlocks;

        for (const FBlockWFC& NeighborBlock : NeighborBlocks)
        {
          bool bIsCompatible = false;

          for (const FBlockWFC& CollapsedBlock : Wave[CurrentIndex])
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

	bool WaveFunctionCollapse::CheckCompatibility(const FBlockWFC& CollapsedBlock, const FBlockWFC& NeighborBlock, const FIntVector& Direction)
	{
		if (Direction == FIntVector(1, 0, 0))
		{
      bool result = CollapsedBlock.SocketFront.CanConnect(NeighborBlock.SocketBack);

      //log 2 sockets and result
			//UE_LOG(LogTemp, Display, TEXT("SocketFront: %s"), *CollapsedBlock.SocketFront.ToString());
			//UE_LOG(LogTemp, Display, TEXT("SocketBack: %s"), *NeighborBlock.SocketBack.ToString());
			//UE_LOG(LogTemp, Display, TEXT("Result: %d"), result);

      return result;
		}
		else if (Direction == FIntVector(-1, 0, 0))
		{
			bool result = CollapsedBlock.SocketBack.CanConnect(NeighborBlock.SocketFront);
      return result;
		}
		else if (Direction == FIntVector(0, 1, 0))
		{
      bool result = CollapsedBlock.SocketRight.CanConnect(NeighborBlock.SocketLeft);
			return result;
		}
		else if (Direction == FIntVector(0, -1, 0))
		{
      bool result = CollapsedBlock.SocketLeft.CanConnect(NeighborBlock.SocketRight);
      return result;
		}
		else if (Direction == FIntVector(0, 0, 1))
		{
      bool result = CollapsedBlock.SocketUp.CanConnect(NeighborBlock.SocketDown);
      return result;
		}
		else if (Direction == FIntVector(0, 0, -1))
		{
      bool result = CollapsedBlock.SocketDown.CanConnect(NeighborBlock.SocketUp);
      return result;
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
}



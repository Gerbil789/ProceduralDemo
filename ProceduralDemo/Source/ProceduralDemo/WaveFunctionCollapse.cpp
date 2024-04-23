#include "WaveFunctionCollapse.h"

const TArray<FIntVector> AWaveFunctionCollapse::Directions = { FIntVector(1, 0, 0),FIntVector(-1, 0, 0), FIntVector(0, 1, 0), FIntVector(0, -1, 0), FIntVector(0, 0, 1), FIntVector(0, 0, -1) };
TArray<WFCBlock> AWaveFunctionCollapse::Blocks;
FIntVector AWaveFunctionCollapse::Size;
TMap<FIntVector, WFCBlock> AWaveFunctionCollapse::CollapsedBlocks;
TMap<FIntVector, TArray<int>> AWaveFunctionCollapse::Grid;


TMap<FIntVector, WFCBlock> AWaveFunctionCollapse::Generate(const TArray<WFCBlock>& blocks, const TMap<FIntVector, WFCBlock>& collapsedBlocks, const FIntVector& size)
{
	Grid.Empty();
	CollapsedBlocks = collapsedBlocks;
	Blocks = blocks;
	Size = size;

	//intialize grid
	for (int X = 0; X < Size.X; X++)
	{
		for (int Y = 0; Y < Size.Y; Y++)
		{
			for (int Z = 0; Z < Size.Z; Z++)
			{
				if(CollapsedBlocks.Contains(FIntVector(X, Y, Z))) continue;

				TArray<int> BlockArray;
				for (int i = 0; i < Blocks.Num(); ++i) {
					BlockArray.Add(i);
				}
				Grid.Emplace(FIntVector(X, Y, Z), BlockArray);
			}
		}
	}

	for (auto& pair : CollapsedBlocks)
	{
		Propagate(pair.Key);
	}

	int tmp = 0;
	while (Grid.IsEmpty() == false && Grid.Num() != 0) {
		if (tmp > 10000) {
			throw std::runtime_error("Infinite loop detected in generation");
			return CollapsedBlocks;
		}

		FIntVector lowestEntropyPosition = GetLowestEntropyPosition();
		Collapse(lowestEntropyPosition);
		Propagate(lowestEntropyPosition);
	}

	return CollapsedBlocks;
}

FIntVector AWaveFunctionCollapse::GetLowestEntropyPosition()
{
	int LowestEntropy = TNumericLimits<int32>::Max();

	TSet<FIntVector> LowestEntropySet = TSet<FIntVector>();

	for (auto BlockArray : Grid)
	{
		int BlockEntropy = BlockArray.Value.Num(); //number of possible blocks at this position

		if (BlockEntropy == 0) {
			Grid.Remove(BlockArray.Key);
			continue;
		}


		if (BlockEntropy < LowestEntropy)
		{
			LowestEntropy = BlockEntropy;
			LowestEntropySet.Empty();
			LowestEntropySet.Add(BlockArray.Key);
		}
		else if (BlockEntropy == LowestEntropy)
		{
			LowestEntropySet.Add(BlockArray.Key);
		}
	}

	if (LowestEntropySet.IsEmpty()) {
		throw std::runtime_error("No lowest entropy set found");
	}

	FIntVector LowestEntropyBlockPosition = LowestEntropySet.Array()[FMath::RandRange(0, LowestEntropySet.Num() - 1)];
	return LowestEntropyBlockPosition;
}

void AWaveFunctionCollapse::Collapse(const FIntVector& position)
{
	int blockId;
	if (Grid[position].Num() > 1)
	{
		//make a pool of values, then pick a random one (block with higher priority have more entries in the pool)
		int pool = 0;
		for (auto i : Grid[position]) {
			pool += Blocks[i].Priority;
		}
		int randomNumber = FMath::RandRange(0, pool);

		for (auto i : Grid[position]) {
			randomNumber -= Blocks[i].Priority;
			if (randomNumber <= 0) {
				blockId = i;
				break;
			}
		}
	}
	else
	{
		blockId = Grid[position][0];
	}

	Grid.Remove(position);
	CollapsedBlocks.Add(position, Blocks[blockId]);
}

void AWaveFunctionCollapse::Propagate(const FIntVector& position)
{
	for (auto Direction : Directions)
	{
		FIntVector newPosition = position + Direction;

		if (!Grid.Contains(newPosition)) continue;

		RemoveUnvalidBlocks(newPosition, Direction);
	}

}

void AWaveFunctionCollapse::RemoveUnvalidBlocks(const FIntVector& position, const FIntVector& direction)
{
	if (!CollapsedBlocks.Contains(position - direction)) return;
	WFCBlock CollapsedBlock = CollapsedBlocks[position - direction];
	TSet<int> BlocksToRemove;

	for (const auto& blockId : Grid[position])
	{
		if ((direction.X == 1 && CollapsedBlock.Right != Blocks[blockId].Left) ||
			(direction.X == -1 && CollapsedBlock.Left != Blocks[blockId].Right) ||
			(direction.Y == 1 && CollapsedBlock.Front != Blocks[blockId].Back) ||
			(direction.Y == -1 && CollapsedBlock.Back != Blocks[blockId].Front) ||
			(direction.Z == 1 && CollapsedBlock.Top != Blocks[blockId].Bottom) ||
			(direction.Z == -1 && CollapsedBlock.Bottom != Blocks[blockId].Top))
		{
			BlocksToRemove.Add(blockId);
		}
	}

	if (BlocksToRemove.Num() == Grid[position].Num()) {
		throw std::runtime_error("No valid blocks to propagate");
	}

	for (auto blockIndex : BlocksToRemove)
	{
		Grid[position].Remove(blockIndex);
	}

	if (Grid[position].Num() == 0) {
		throw std::runtime_error("No blocks left in grid");
	}
}



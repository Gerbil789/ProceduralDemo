#include "WFCBuildingModule.h"

void AWFCBuildingModule::Process(TMap<FIntVector, WFCBlock>& determinedBlocks, const TArray<WFCBlock>& blocks, const FIntVector& size)
{
	// get subset of blocks with building type
	TArray<WFCBlock> buildingBlokcs;
	for (const WFCBlock& block : blocks) {
		if (block.Type != BlockType::ROAD) {
			buildingBlokcs.Add(block);
		}
	}

	if (buildingBlokcs.Num() == 0) throw std::runtime_error("No building blocks found");
	//UE_LOG(LogTemp, Warning, TEXT("Building blocks: %d"), buildingBlokcs.Num())

	Grid = TMap<FIntVector, TArray<int>>();
	DeterminedBlocks = determinedBlocks;
	Size = size;
	Blocks = buildingBlokcs;

	//intialize grid (easy to check if the position is in bounds)
	for (int X = 0; X < Size.X; X++)
	{
		for (int Y = 0; Y < Size.Y; Y++)
		{
			for (int Z = 0; Z < Size.Z; Z++)
			{
				if (DeterminedBlocks.Contains(FIntVector(X, Y, Z))) continue;

				TArray<int> BlockArray;
				for (int i = 0; i < Blocks.Num(); ++i) {
					BlockArray.Add(i);
				}
				Grid.Emplace(FIntVector(X, Y, Z), BlockArray);
			}
		}
	}

	/*for (int X = -1; X <= Size.X; X++)
	{
		for (int Y = -1; Y <= Size.Y; Y++)
		{
			for (int Z = 0; Z <= Size.Z; Z++)
			{
				if (X == -1 || Y == -1 || X == Size.X || Y == Size.Y)
				{
					if (Z == 0) {
						DeterminedBlocks.Add(FIntVector(X, Y, Z), Blocks[2]);
					}
					else {
						DeterminedBlocks.Add(FIntVector(X, Y, Z), Blocks[0]);
					}

					
				}

				
			}
		}
	}*/

	for(int X = 0; X < Size.X; X++)
	{
		for (int Y = 0; Y < Size.Y; Y++)
		{
			DeterminedBlocks.Add(FIntVector(X, Y, Size.Z), Blocks[0]);
			DeterminedBlocks.Add(FIntVector(X, Y, -1), Blocks[1]);
		}
	}


	for (auto& pair : DeterminedBlocks)
	{
		Propagate(pair.Key);
	}

	//collapse
	int infiniteLoopDetector = 0;
	while (Grid.IsEmpty() == false && Grid.Num() != 0) {
		if (infiniteLoopDetector > 10000) throw std::runtime_error("Infinite loop detected in generation");

		FIntVector lowestEntropyPosition = GetLowestEntropyPosition();
		Collapse(lowestEntropyPosition);
		Propagate(lowestEntropyPosition);
	}

	for (int X = 0; X < Size.X; X++)
	{
		for (int Y = 0; Y < Size.Y; Y++)
		{
			DeterminedBlocks.Remove(FIntVector(X, Y, Size.Z));
			DeterminedBlocks.Remove(FIntVector(X, Y, -1));
		}
	}

	determinedBlocks = DeterminedBlocks;
}

FIntVector AWFCBuildingModule::GetLowestEntropyPosition()
{
	int LowestEntropy = TNumericLimits<int32>::Max();

	TSet<FIntVector> LowestEntropySet = TSet<FIntVector>();

	for (auto BlockArray : Grid)
	{
		int BlockEntropy = BlockArray.Value.Num(); //number of possible blocks at this position

		if (BlockEntropy == 0) {
			Grid.Remove(BlockArray.Key);
			UE_LOG(LogTemp, Warning, TEXT("Block at %s has no possible blocks"), *BlockArray.Key.ToString())
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

void AWFCBuildingModule::Collapse(const FIntVector& position)
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
	DeterminedBlocks.Add(position, Blocks[blockId]);
}

void AWFCBuildingModule::Propagate(const FIntVector& position)
{
	for (auto Direction : Directions)
	{
		FIntVector newPosition = position + Direction;

		if (!Grid.Contains(newPosition)) continue;

		RemoveInvalidBlocks(newPosition, Direction);
	}
}

void AWFCBuildingModule::RemoveInvalidBlocks(const FIntVector& position, const FIntVector& direction)
{
	if (!DeterminedBlocks.Contains(position - direction)) return;
	WFCBlock CollapsedBlock = DeterminedBlocks[position - direction];
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

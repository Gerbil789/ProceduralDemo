#include "BuildingModule.h"
#include "ProceduralGridActor.h"

void ABuildingModule::Process()
{
	// initialization
	UncollapsedGrid = TMap<FIntVector, TArray<int>>();
	Wave = std::stack<FIntVector>();
	InWave = TSet<FIntVector>();
	Grid = GridActor->Grid;
	CollapsedCount = Grid.Num();

	//define border blocks
	/*Block topBorderBlock = Block();
	topBorderBlock.Bottom = Block::ParseVertical("0i", 0);

	Block bottomBorderBlock = Block();
	bottomBorderBlock.Top = Block::ParseVertical("1i", 0);

	Block emptyBlock = Block();
	emptyBlock.Left = Block::ParseHorizontal("0s");
	emptyBlock.Right = Block::ParseHorizontal("0s");
	emptyBlock.Front = Block::ParseHorizontal("0s");
	emptyBlock.Back = Block::ParseHorizontal("0s");
	emptyBlock.Top = Block::ParseVertical("0i", 0);
	emptyBlock.Bottom = Block::ParseVertical("0i", 0);

	Block sidewalkBlock = Block();
	sidewalkBlock.Left = Block::ParseHorizontal("4s");
	sidewalkBlock.Right = Block::ParseHorizontal("4s");
	sidewalkBlock.Front = Block::ParseHorizontal("4s");
	sidewalkBlock.Back = Block::ParseHorizontal("4s");*/


	for (int X = 0; X < GridActor->Size.X; X++)
	{
		for (int Y = 0; Y < GridActor->Size.Y; Y++)
		{
			for (int Z = 0; Z < GridActor->Size.Z; Z++)
			{
				if (Grid.Contains(FIntVector(X, Y, Z))) continue;

				TArray<int> BlockArray = BlockIds;
				UncollapsedGrid.Emplace(FIntVector(X, Y, Z), BlockArray);
			}

			//add top and bottom border blocks
			//DeterminedBlocks.Add(FIntVector(X, Y, Size.Z), topBorderBlock);
			//DeterminedBlocks.Add(FIntVector(X, Y, -1), bottomBorderBlock);
		}
	}

	/*for (int X = 0; X < Size.X; X++)
	{
		for (int Y = 0; Y < Size.Y; Y++)
		{
			for (auto Z : { 1, Size.Z - 1 }) {

				TSet<int> BlocksToRemove;
				for (auto blockId : Grid[FIntVector(X, Y, Z)]) {
					if (X == 0 && Blocks[blockId].Left != emptyBlock.Right ||
						X == Size.X - 1 && Blocks[blockId].Right != emptyBlock.Left ||
						Y == 0 && Blocks[blockId].Back != emptyBlock.Front ||
						Y == Size.Y - 1 && Blocks[blockId].Front != emptyBlock.Back)
					{
						BlocksToRemove.Add(blockId);
					}
				}
				for (auto blockId : BlocksToRemove) {
					Grid[FIntVector(X, Y, Z)].Remove(blockId);
				}
			}

			TSet<int> BlocksToRemove;
			for (auto blockId : Grid[FIntVector(X, Y, Size.Z - 1)]) {
				if (Blocks[blockId].Top != emptyBlock.Bottom)
				{
					BlocksToRemove.Add(blockId);
				}
			}
			for (auto blockId : BlocksToRemove) {
				Grid[FIntVector(X, Y, Size.Z - 1)].Remove(blockId);
			}

			BlocksToRemove.Empty();
			for (auto blockId : Grid[FIntVector(X, Y, 0)]) {
				if (X == 0 && Blocks[blockId].Left != sidewalkBlock.Right ||
					X == Size.X - 1 && Blocks[blockId].Right != sidewalkBlock.Left ||
					Y == 0 && Blocks[blockId].Back != sidewalkBlock.Front ||
					Y == Size.Y - 1 && Blocks[blockId].Front != sidewalkBlock.Back)
				{
					BlocksToRemove.Add(blockId);
				}
			}
			for (auto blockId : BlocksToRemove) {
				Grid[FIntVector(X, Y, 0)].Remove(blockId);
			}

		
		}
	}*/

	UE_LOG(LogTemp, Warning, TEXT("Grid size: %d"), Grid.Num());

	for (auto& pair : Grid)
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid: %s"), *pair.Key.ToString());
		Propagate(pair.Key);
	}

	
	//collapse
	int tmp = 0;
	int total = GridActor->Size.X * GridActor->Size.Y * GridActor->Size.Z;
	while (CollapsedCount != total) {
		if (tmp++ > 1000) throw std::runtime_error("Infinite loop detected in process (1000)");
		FIntVector position = GetLowestEntropyPosition();
		Collapse(position);
		Propagate(position);
	}

	//remove all border blocks (check size)
	//for (auto& pair : Grid)
	//{
	//	if (pair.Key.X == -1 || pair.Key.X == GridActor->Size.X || pair.Key.Y == -1 || pair.Key.Y == GridActor->Size.Y || pair.Key.Z == -1 || pair.Key.Z == GridActor->Size.Z) {
	//		Grid.Remove(pair.Key);
	//	}
	//}

	GridActor->Grid = Grid;
}

FIntVector ABuildingModule::GetLowestEntropyPosition()
{
	int LowestEntropy = TNumericLimits<int32>::Max();
	TSet<FIntVector> LowestEntropySet = TSet<FIntVector>();

	for (auto BlockArray : UncollapsedGrid)
	{
		int BlockEntropy = BlockArray.Value.Num(); //number of possible blocks at this position

		if (BlockEntropy == 0) {
			UncollapsedGrid.Remove(BlockArray.Key);
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

TArray<FIntVector> ABuildingModule::GetNeighbors(const FIntVector& position)
{
	TArray<FIntVector> Neighbors;
	for (auto Direction : Directions)
	{
		FIntVector newPos = position + Direction;
		if (Grid.Contains(newPos)) continue;
		if (Utility::IsInBounds(newPos, GridActor->Size)) {
			Neighbors.Add(newPos);
		}
	}
	return Neighbors;
}

int ABuildingModule::PickBlock(const FIntVector& position)
{
	//make a pool of values, then pick a random one (block with higher priority have more entries in the pool)
	int Pool = 0;
	for (auto i : UncollapsedGrid[position]) {
		Pool += GridActor->Blocks[i].Priority;
	}
	int Random = FMath::RandRange(0, Pool);

	for (auto i : UncollapsedGrid[position]) {
		Random -= GridActor->Blocks[i].Priority;
		if (Random <= 0) {
			return i;
			break;
		}
	}
	throw std::runtime_error("No block picked");
}

void ABuildingModule::Collapse(const FIntVector& position)
{
	//TSet<int> IncompatibleBlocks = GetIncompatibleBlocks(position);
	//if (IncompatibleBlocks.Num() != 0) UE_LOG(LogTemp, Error, TEXT("Incompatible blocks: %d"), IncompatibleBlocks.Num())

	//	if (IncompatibleBlocks.Num() == UncollapsedGrid[position].Num()) {
	//		throw std::runtime_error("No blocks to collapse");
	//	}

	//for (auto BlockId : IncompatibleBlocks)
	//{
	//	UncollapsedGrid[position].Remove(BlockId);
	//}

	int blockId = PickBlock(position); //pick a random block (with priority weights)
	UncollapsedGrid.Remove(position);
	Grid.Add(position, blockId);
	CollapsedCount++;
}

void ABuildingModule::Propagate(const FIntVector& position)
{
	auto Neighbors = GetNeighbors(position);
	for (auto Neighbor : Neighbors)
	{
		Wave.push(Neighbor);
		InWave.Add(Neighbor);
	}

	int tmp = 0;
	while (!Wave.empty())
	{
		if (tmp++ > 1000) throw std::runtime_error("Infinite loop detected in propagation (1000)");
		FIntVector CurrentPosition = Wave.top();
		Wave.pop();

		TSet<int> BlocksToRemove = GetIncompatibleBlocks(CurrentPosition);
	
		//UE_LOG(LogTemp, Warning, TEXT("Blocks to remove: %d"), BlocksToRemove.Num())

		for (auto BlockId : BlocksToRemove)
		{
			UncollapsedGrid[CurrentPosition].Remove(BlockId);
		}

		if (UncollapsedGrid[CurrentPosition].IsEmpty()) {

			throw std::runtime_error("No blocks to propagate");
		}

		if (!BlocksToRemove.IsEmpty()) {
			auto neighbors = GetNeighbors(CurrentPosition);
			for (auto neighbor : neighbors)
			{
				if (InWave.Contains(neighbor)) continue;
				Wave.push(neighbor);
				InWave.Add(neighbor);
			}
		}

		if (UncollapsedGrid[CurrentPosition].Num() == 1) {
			Collapse(CurrentPosition);
		}

		InWave.Remove(CurrentPosition);
	}
	InWave.Empty();
}

TSet<int> ABuildingModule::GetIncompatibleBlocks(const FIntVector& position)
{
	if (Grid.Contains(position) || UncollapsedGrid[position].IsEmpty()) throw std::runtime_error("Position already collapsed");
	TSet<int> BlocksToRemove = TSet<int>();

	for (auto Direction : Directions)
	{
		//get collapsed block to compare
		if (!Grid.Contains(position + Direction)) continue;
		Block BlockToCompare = GridActor->Blocks[Grid[position + Direction]];

		//compare block socktes
		for (auto BlockId : UncollapsedGrid[position]) {
			if (!BlockToCompare.IsValidNeighbor(GridActor->Blocks[BlockId], Direction * -1)) {
				BlocksToRemove.Add(BlockId);
			}
		}
	}

	return BlocksToRemove;
}



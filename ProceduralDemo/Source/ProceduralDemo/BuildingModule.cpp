#include "BuildingModule.h"
#include "ProceduralGridActor.h"

void ABuildingModule::Process()
{
}

//void AWFCBuildingModule::Process(TMap<FIntVector, int>& grid, const FIntVector& size)
//{
//	// initialization
//	AModuleBase::Process(grid, size);
//	UncollapsedGrid = TMap<FIntVector, TArray<int>>();
//	Wave = std::stack<FIntVector>();
//	InWave = TSet<FIntVector>();
//
//	//define border blocks
//	WFCBlock topBorderBlock = WFCBlock();
//	topBorderBlock.Bottom = WFCBlock::ParseVertical("0i", 0);
//
//	WFCBlock bottomBorderBlock = WFCBlock();
//	bottomBorderBlock.Top = WFCBlock::ParseVertical("1i", 0);
//
//	WFCBlock emptyBlock = WFCBlock();
//	emptyBlock.Left = WFCBlock::ParseHorizontal("0s");
//	emptyBlock.Right = WFCBlock::ParseHorizontal("0s");
//	emptyBlock.Front = WFCBlock::ParseHorizontal("0s");
//	emptyBlock.Back = WFCBlock::ParseHorizontal("0s");
//	emptyBlock.Top = WFCBlock::ParseVertical("0i", 0);
//	emptyBlock.Bottom = WFCBlock::ParseVertical("0i", 0);
//
//	WFCBlock sidewalkBlock = WFCBlock();
//	sidewalkBlock.Left = WFCBlock::ParseHorizontal("4s");
//	sidewalkBlock.Right = WFCBlock::ParseHorizontal("4s");
//	sidewalkBlock.Front = WFCBlock::ParseHorizontal("4s");
//	sidewalkBlock.Back = WFCBlock::ParseHorizontal("4s");
//
//
//	for (int X = 0; X < Size.X; X++)
//	{
//		for (int Y = 0; Y < Size.Y; Y++)
//		{
//			for (int Z = 0; Z < Size.Z; Z++)
//			{
//				if (Grid.Contains(FIntVector(X, Y, Z))) continue;
//
//				TArray<int> BlockArray;
//				for (int i = 0; i < Blocks.Num(); ++i) {
//					BlockArray.Add(i);
//				}
//				Grid.Emplace(FIntVector(X, Y, Z), BlockArray);
//			}
//
//			//add top and bottom border blocks
//			//DeterminedBlocks.Add(FIntVector(X, Y, Size.Z), topBorderBlock);
//			//DeterminedBlocks.Add(FIntVector(X, Y, -1), bottomBorderBlock);
//		}
//	}
//
//	/*for (int X = 0; X < Size.X; X++)
//	{
//		for (int Y = 0; Y < Size.Y; Y++)
//		{
//			for (auto Z : { 1, Size.Z - 1 }) {
//
//				TSet<int> BlocksToRemove;
//				for (auto blockId : Grid[FIntVector(X, Y, Z)]) {
//					if (X == 0 && Blocks[blockId].Left != emptyBlock.Right ||
//						X == Size.X - 1 && Blocks[blockId].Right != emptyBlock.Left ||
//						Y == 0 && Blocks[blockId].Back != emptyBlock.Front ||
//						Y == Size.Y - 1 && Blocks[blockId].Front != emptyBlock.Back)
//					{
//						BlocksToRemove.Add(blockId);
//					}
//				}
//				for (auto blockId : BlocksToRemove) {
//					Grid[FIntVector(X, Y, Z)].Remove(blockId);
//				}
//			}
//
//			TSet<int> BlocksToRemove;
//			for (auto blockId : Grid[FIntVector(X, Y, Size.Z - 1)]) {
//				if (Blocks[blockId].Top != emptyBlock.Bottom)
//				{
//					BlocksToRemove.Add(blockId);
//				}
//			}
//			for (auto blockId : BlocksToRemove) {
//				Grid[FIntVector(X, Y, Size.Z - 1)].Remove(blockId);
//			}
//
//			BlocksToRemove.Empty();
//			for (auto blockId : Grid[FIntVector(X, Y, 0)]) {
//				if (X == 0 && Blocks[blockId].Left != sidewalkBlock.Right ||
//					X == Size.X - 1 && Blocks[blockId].Right != sidewalkBlock.Left ||
//					Y == 0 && Blocks[blockId].Back != sidewalkBlock.Front ||
//					Y == Size.Y - 1 && Blocks[blockId].Front != sidewalkBlock.Back)
//				{
//					BlocksToRemove.Add(blockId);
//				}
//			}
//			for (auto blockId : BlocksToRemove) {
//				Grid[FIntVector(X, Y, 0)].Remove(blockId);
//			}
//
//		
//		}
//	}*/
//
//	for (auto& pair : Grid)
//	{
//		Propagate(pair.Key);
//	}
//
//	
//	//collapse
//	int tmp = 0;
//	while (!Grid.IsEmpty()) {
//		if (tmp++ > 1000) throw std::runtime_error("Infinite loop detected in Prorcess (1000)");
//		FIntVector position = GetLowestEntropyPosition();
//		Collapse(position);
//		Propagate(position);
//	}
//
//	//remove all border blocks (check size)
//	for (auto& pair : Grid)
//	{
//		if (pair.Key.X == -1 || pair.Key.X == Size.X || pair.Key.Y == -1 || pair.Key.Y == Size.Y || pair.Key.Z == -1 || pair.Key.Z == Size.Z) {
//			Grid.Remove(pair.Key);
//		}
//	}
//
//	grid = Grid;
//}
//
//FIntVector AWFCBuildingModule::GetLowestEntropyPosition()
//{
//	int LowestEntropy = TNumericLimits<int32>::Max();
//	TSet<FIntVector> LowestEntropySet = TSet<FIntVector>();
//
//	for (auto BlockArray : UncollapsedGrid)
//	{
//		int BlockEntropy = BlockArray.Value.Num(); //number of possible blocks at this position
//
//		if (BlockEntropy == 0) {
//			UncollapsedGrid.Remove(BlockArray.Key);
//			UE_LOG(LogTemp, Warning, TEXT("Block at %s has no possible blocks"), *BlockArray.Key.ToString())
//				continue;
//		}
//
//		if (BlockEntropy < LowestEntropy)
//		{
//			LowestEntropy = BlockEntropy;
//			LowestEntropySet.Empty();
//			LowestEntropySet.Add(BlockArray.Key);
//		}
//		else if (BlockEntropy == LowestEntropy)
//		{
//			LowestEntropySet.Add(BlockArray.Key);
//		}
//	}
//
//	if (LowestEntropySet.IsEmpty()) {
//		throw std::runtime_error("No lowest entropy set found");
//	}
//
//	FIntVector LowestEntropyBlockPosition = LowestEntropySet.Array()[FMath::RandRange(0, LowestEntropySet.Num() - 1)];
//	return LowestEntropyBlockPosition;
//}
//
//TArray<FIntVector> AWFCBuildingModule::GetNeighbors(const FIntVector& position)
//{
//	TArray<FIntVector> Neighbors;
//	for (auto Direction : Directions)
//	{
//		FIntVector Neighbor = position + Direction;
//		if (Grid.Contains(Neighbor)) {
//			Neighbors.Add(Neighbor);
//		}
//	}
//	return Neighbors;
//}
//
//int AWFCBuildingModule::PickBlock(const FIntVector& position)
//{
//	//make a pool of values, then pick a random one (block with higher priority have more entries in the pool)
//	int Pool = 0;
//	for (auto i : UncollapsedGrid[position]) {
//		Pool += Blocks[i].Priority;
//	}
//	int Random = FMath::RandRange(0, Pool);
//
//	for (auto i : UncollapsedGrid[position]) {
//		Random -= Blocks[i].Priority;
//		if (Random <= 0) {
//			return i;
//			break;
//		}
//	}
//	throw std::runtime_error("No block picked");
//}
//
//void AWFCBuildingModule::Collapse(const FIntVector& position)
//{
//	auto IncompatibleBlocks = GetIncompatibleBlocks(position);
//	if (IncompatibleBlocks.Num() != 0) UE_LOG(LogTemp, Error, TEXT("Incompatible blocks: %d"), IncompatibleBlocks.Num())
//
//		if (IncompatibleBlocks.Num() == UncollapsedGrid[position].Num()) {
//			throw std::runtime_error("No blocks to collapse");
//		}
//
//	for (auto BlockId : IncompatibleBlocks)
//	{
//		UncollapsedGrid[position].Remove(BlockId);
//	}
//
//	int blockId = PickBlock(position); //pick a random block (with priority weights)
//	UncollapsedGrid.Remove(position);
//	Grid.Add(position, blockId);
//}
//
//void AWFCBuildingModule::Propagate(const FIntVector& position)
//{
//	auto Neighbors = GetNeighbors(position);
//	for (auto Neighbor : Neighbors)
//	{
//		Wave.push(Neighbor);
//		InWave.Add(Neighbor);
//	}
//
//	int tmp = 0;
//	while (!Wave.empty())
//	{
//		if (tmp++ > 1000) throw std::runtime_error("Infinite loop detected (1000)");
//		FIntVector CurrentPosition = Wave.top();
//		Wave.pop();
//
//		//get blocks to remove
//		auto BlocksToRemove = GetIncompatibleBlocks(CurrentPosition);
//		bool changed = BlocksToRemove.Num() > 0;
//		//UE_LOG(LogTemp, Warning, TEXT("Blocks to remove: %d"), BlocksToRemove.Num())
//
//		for (auto BlockId : BlocksToRemove)
//		{
//			UncollapsedGrid[CurrentPosition].Remove(BlockId);
//		}
//
//		//log current position count
//		//UE_LOG(LogTemp, Warning, TEXT("Current position count: %d"), Grid[CurrentPosition].Num())
//
//		if (UncollapsedGrid[CurrentPosition].IsEmpty()) {
//
//			throw std::runtime_error("No blocks to propagate");
//		}
//
//		if (changed) {
//			auto neighbors = GetNeighbors(CurrentPosition);
//			for (auto neighbor : neighbors)
//			{
//				if (InWave.Contains(neighbor)) continue;
//				Wave.push(neighbor);
//				InWave.Add(neighbor);
//			}
//		}
//
//		if (UncollapsedGrid[CurrentPosition].Num() == 1) {
//			Collapse(CurrentPosition);
//		}
//
//		InWave.Remove(CurrentPosition);
//	}
//	InWave.Empty();
//}
//
//TSet<int> AWFCBuildingModule::GetIncompatibleBlocks(const FIntVector& position)
//{
//	if (!Grid.Contains(position) || UncollapsedGrid[position].IsEmpty()) throw std::runtime_error("Position not in grid (already collapsed)");
//
//	TSet<int> BlocksToRemove;
//
//	for (auto Direction : Directions)
//	{
//		if (!Grid.Contains(position + Direction)) continue;
//		WFCBlock BlockToCompare = Blocks[Grid[position + Direction]];
//
//		//compare block socktes
//		for (auto BlockId : UncollapsedGrid[position]) {
//			auto Block = Blocks[BlockId];
//			if (Direction.X == 1) {
//				if (Block.Right == BlockToCompare.Left) continue;
//			}
//			else if (Direction.X == -1) {
//				if (Block.Left == BlockToCompare.Right) continue;
//			}
//			else if (Direction.Y == 1) {
//				if (Block.Front == BlockToCompare.Back) continue;
//			}
//			else if (Direction.Y == -1) {
//				if (Block.Back == BlockToCompare.Front) continue;
//			}
//			else if (Direction.Z == 1) {
//				if (Block.Top == BlockToCompare.Bottom) continue;
//			}
//			else if (Direction.Z == -1) {
//				if (Block.Bottom == BlockToCompare.Top) continue;
//			}
//
//			BlocksToRemove.Add(BlockId);
//		}
//	}
//
//	return BlocksToRemove;
//}



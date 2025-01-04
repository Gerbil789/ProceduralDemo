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

	int emptyBlockId = GridActor->FindBlocks("Empty")[0];
	int sidewalkBlockId = GridActor->FindBlocks("Sidewalk")[0];

	//initialize uncollapsed grid
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
			//Grid.Add(FIntVector(X, Y, -1), bottomBorderBlock);
		}
	}

	//propagate already existing blocks
	for (auto& pair : Grid)
	{
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

	//initialize border blocks
	for (int X = -1; X <= GridActor->Size.X; X++)
	{
		for (int Y = -1; Y <= GridActor->Size.Y; Y++)
		{
			for (int Z = 0; Z < GridActor->Size.Z; Z++)
			{
				if (X == -1 || X == GridActor->Size.X || Y == -1 || Y == GridActor->Size.Y) {
					if (Z == 0) Grid.Add(FIntVector(X, Y, Z), sidewalkBlockId);
					else Grid.Add(FIntVector(X, Y, Z), emptyBlockId);
				}

				if(X == 0 || X == GridActor->Size.X - 1 || Y == 0 || Y == GridActor->Size.Y - 1) {
					if (GridActor->Grid.Contains(FIntVector(X, Y, Z))) continue; //skip already existing blocks
					Grid.Remove(FIntVector(X, Y, Z));
					CollapsedCount--;

					//corners
					if ((X == 0 && Y == 0) || (X == 0 && Y == GridActor->Size.Y - 1) || (X == GridActor->Size.X - 1 && Y == 0) || (X == GridActor->Size.X - 1 && Y == GridActor->Size.Y - 1)) {
						if(Z == 0) {
							TArray<int> BlockArray = TArray<int>{ sidewalkBlockId };
							UncollapsedGrid.Emplace(FIntVector(X, Y, Z), BlockArray);
						}
						else {
							TArray<int> BlockArray = TArray<int>{ emptyBlockId };
							UncollapsedGrid.Emplace(FIntVector(X, Y, Z), BlockArray);
						}
					}
					else {
						TArray<int> BlockArray = BlockIds;
						UncollapsedGrid.Emplace(FIntVector(X, Y, Z), BlockArray);
					}
				}
			}
		}
	}

	for (int X = -1; X <= GridActor->Size.X; X++)
	{
		for (int Y = -1; Y <= GridActor->Size.Y; Y++)
		{
			for (int Z = 0; Z < GridActor->Size.Z; Z++)
			{
				if (X == -1 || X == GridActor->Size.X || Y == -1 || Y == GridActor->Size.Y) {
					Propagate(FIntVector(X, Y, Z));
				}
			}
		}
	}

	//collapse borders
	tmp = 0;
	while (CollapsedCount != total) {
		if (tmp++ > 1000) throw std::runtime_error("Infinite loop detected in process (1000)");
		FIntVector position = GetLowestEntropyPosition();
		Collapse(position);
		Propagate(position);
	}

	//remove all extra border blocks
	for (int X = -1; X <= GridActor->Size.X; X++)
	{
		for (int Y = -1; Y <= GridActor->Size.Y; Y++)
		{
			for (int Z = 0; Z < GridActor->Size.Z; Z++)
			{
				if (X == -1 || X == GridActor->Size.X || Y == -1 || Y == GridActor->Size.Y) {
					Grid.Remove(FIntVector(X, Y, Z));
				}
			}
		}
	}

	//assign grid to grid actor
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
		if (tmp++ > 10000) throw std::runtime_error("Infinite loop detected in propagation (10000)");
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
			if (!BlockToCompare.IsValidNeighbor(GridActor->Blocks[BlockId], Direction)) {
				BlocksToRemove.Add(BlockId);
			}
		}
	}

	return BlocksToRemove;
}



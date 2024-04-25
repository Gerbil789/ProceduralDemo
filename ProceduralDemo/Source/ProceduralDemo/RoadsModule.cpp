#include "RoadsModule.h"

void ARoadsModule::Process(TMap<FIntVector, WFCBlock>& determinedBlocks, const TArray<WFCBlock>& blocks, const FIntVector& size)
{
	AModuleBase::Process(determinedBlocks, blocks, size);

	// get subset of blocks with roads only
	TArray<WFCBlock> roadBlocks;
	for (const WFCBlock& block : Blocks) {
		if (block.Type == BlockType::ROAD) {
			roadBlocks.Add(block);
		}
	}

	if (roadBlocks.Num() == 0) throw std::runtime_error("No road blocks found");

	TSet<FIntVector> AvailablePositions;
	for (int x = 0; x < Size.X; x++) {
		for (int y = 0; y < Size.Y; y++) {
			if (DeterminedBlocks.Contains(FIntVector(x, y, 0))) continue;
			AvailablePositions.Add(FIntVector(x, y, 0));
		}
	}

	if (AvailablePositions.IsEmpty()) return;

	std::queue<FIntVector> Queue;
	Queue.push(AvailablePositions.Array()[FMath::RandRange(0, AvailablePositions.Num() - 1)]);

	while (!Queue.empty()) {
		FIntVector current = Queue.front();
		Queue.pop();

		if (DeterminedBlocks.Contains(current)) continue;

		TArray<WFCBlock> roadBlocksCopy = roadBlocks;
		
		WFCBlock block = GetRandomBlock(roadBlocksCopy);

		while (!IsValid(block, current)) {
			roadBlocksCopy.Remove(block);
			if (roadBlocksCopy.Num() == 0) {
				throw std::runtime_error("No valid blocks found");
			}
			block = GetRandomBlock(roadBlocksCopy);
		}

		DeterminedBlocks.Add(current, block);

		for (auto dir : Directions) {
			FIntVector newPosition = current + dir;
			if (newPosition.X < 0 || newPosition.X >= Size.X || newPosition.Y < 0 || newPosition.Y >= Size.Y) continue;
			if (DeterminedBlocks.Contains(newPosition)) continue;

			if ((dir.X == 1 && block.Right.Connector == 3) ||
				(dir.X == -1 && block.Left.Connector == 3) ||
				(dir.Y == 1 && block.Front.Connector == 3) ||
				(dir.Y == -1 && block.Back.Connector == 3)) {
				Queue.push(newPosition);
			}
		}
	}

	determinedBlocks = DeterminedBlocks;
}

WFCBlock ARoadsModule::GetRandomBlock(const TArray<WFCBlock>& blocks)
{
	if(blocks.Num() == 0) throw std::runtime_error("No blocks to process");

	int pool = 0;
	for (auto block : blocks) {
		pool += block.Priority;
	}
	int randomNumber = FMath::RandRange(0, pool);

	for (auto block : blocks) {
		randomNumber -= block.Priority;
		if (randomNumber <= 0) {
			return block;
		}
	}

	return blocks[0];
	
}

bool ARoadsModule::IsValid(WFCBlock block, FIntVector position)
{
	for (auto dir : Directions)
	{
		if (!DeterminedBlocks.Contains(position + dir)) continue;

		WFCBlock neighbour = DeterminedBlocks[position + dir];
		if (dir.X == 1) {
			if (block.Right.Connector != neighbour.Left.Connector) return false;
		}
		else if (dir.X == -1) {
			if (block.Left.Connector != neighbour.Right.Connector) return false;
		}
		else if (dir.Y == 1) {
			if (block.Front.Connector != neighbour.Back.Connector) return false;
		}
		else if (dir.Y == -1) {
			if (block.Back.Connector != neighbour.Front.Connector) return false;
		}
	}
	return true;
}

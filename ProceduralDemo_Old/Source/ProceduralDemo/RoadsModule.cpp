#include "RoadsModule.h"
#include "ProceduralGridActor.h"

void ARoadsModule::Process()
{
	TSet<FIntVector> AvailablePositions;
	for (int x = 0; x < GridActor->Size.X; x++) {
		for (int y = 0; y < GridActor->Size.Y; y++) {
			if (GridActor->Grid.Contains(FIntVector(x, y, 0))) continue;
			AvailablePositions.Add(FIntVector(x, y, 0));
		}
	}

	if (AvailablePositions.IsEmpty()) return;

	std::queue<FIntVector> Queue;
	// push random position to start
	Queue.push(AvailablePositions.Array()[FMath::RandRange(0, AvailablePositions.Num() - 1)]);

	while (!Queue.empty()) {
		FIntVector current = Queue.front();
		Queue.pop();

		if (GridActor->Grid.Contains(current)) continue;

		TArray<int> roadBlocksCopy = BlockIds;
		int blockId = GetRandomBlock(roadBlocksCopy);

		while (!IsValid(blockId, current)) {
			roadBlocksCopy.Remove(blockId);
			if (roadBlocksCopy.Num() == 0) {
				throw std::runtime_error("No valid blocks found");
			}
			blockId = GetRandomBlock(roadBlocksCopy);
		}

		GridActor->Grid.Add(current, blockId);
		Block block = GridActor->Blocks[blockId];

		for (auto dir : Directions) {
			FIntVector newPosition = current + dir;
			if (newPosition.X < 0 || newPosition.X >= GridActor->Size.X || newPosition.Y < 0 || newPosition.Y >= GridActor->Size.Y) continue;
			if (GridActor->Grid.Contains(newPosition)) continue;

			if ((dir.X == 1 && block.Right.Connector == 2) ||
				(dir.X == -1 && block.Left.Connector == 2) ||
				(dir.Y == 1 && block.Front.Connector == 2) ||
				(dir.Y == -1 && block.Back.Connector == 2)) {
				Queue.push(newPosition);
			}
		}
	}
}

int ARoadsModule::GetRandomBlock(const TArray<int>& ids)
{
	if(ids.Num() == 0) throw std::runtime_error("No blocks to process");

	int pool = 0;
	for (auto id : ids) {
		pool += GridActor->Blocks[id].Priority;
	}
	int randomNumber = FMath::RandRange(0, pool);

	for (auto id : ids) {
		randomNumber -= GridActor->Blocks[id].Priority;
		if (randomNumber <= 0) {
			return id;
		}
	}

	return ids[0];
}

bool ARoadsModule::IsValid(int id, FIntVector position)
{
	for (auto dir : Directions)
	{
		if (!GridActor->Grid.Contains(position + dir)) continue;

		Block neighbour = GridActor->Blocks[GridActor->Grid[position + dir]];
		Block block = GridActor->Blocks[id];
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

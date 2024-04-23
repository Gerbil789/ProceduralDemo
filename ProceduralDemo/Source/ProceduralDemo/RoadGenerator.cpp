#include "RoadGenerator.h"

TMap<FIntVector, WFCBlock> RoadGenerator::GenerateRoads(const TArray<WFCBlock>& blocks, const FIntVector& size)
{
	if (blocks.Num() == 0)
	{
		throw std::runtime_error("No blocks to generate roads");
	}

	FIntVector current = FIntVector(0, FMath::RandRange(0, size.Y - 1), 0);
	FIntVector direction = FIntVector(1, 0, 0);
	TMap<FIntVector, WFCBlock> Roads;

	while (current.X >= 0 && current.X < size.X && current.Y >= 0 && current.Y < size.Y){
		Roads.Add(current, blocks[1]);
		current += direction;
	}

	return Roads;
}

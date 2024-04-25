#include "FillSidewalkModule.h"

void AFillSidewalkModule::Process(TMap<FIntVector, WFCBlock>& determinedBlocks, const TArray<WFCBlock>& blocks, const FIntVector& size)
{
	WFCBlock sidewalkBlock = FindBlock(blocks, "Sidewalk");

	for (int x = 0; x < size.X; x++) {
		for (int y = 0; y < size.Y; y++) {
			if (determinedBlocks.Contains(FIntVector(x, y, 0))) continue;
			determinedBlocks.Add(FIntVector(x, y, 0), sidewalkBlock);
		}
	}
}

WFCBlock AFillSidewalkModule::FindBlock(const TArray<WFCBlock>& blocks, const FString& name)
{
	for (const WFCBlock& block : blocks) {
		if (block.Name == name) {
			return block;
		}
	}

	throw std::runtime_error("Block not found");
}

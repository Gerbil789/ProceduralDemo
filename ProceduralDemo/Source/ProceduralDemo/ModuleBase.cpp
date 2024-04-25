#include "ModuleBase.h"

AModuleBase::AModuleBase()
{

}

void AModuleBase::Process(TMap<FIntVector, WFCBlock>& determinedBlocks, const TArray<WFCBlock>& blocks, const FIntVector& size)
{
	if (blocks.IsEmpty()) {
		throw std::runtime_error("No blocks to process");
		return;
	}

	if (size.X <= 0 || size.Y <= 0 || size.Z <= 0) {
		throw std::runtime_error("Invalid size");
		return;
	}

	Blocks = blocks;
	DeterminedBlocks = determinedBlocks;
	Size = size;
}

#pragma once

#include "CoreMinimal.h"
#include "WFC_Block.h"

class PROCEDURALALGORITHMS_API WaveFunctionCollapse
{
public:
	WaveFunctionCollapse() = default;
	void SetBlocks(const TArray<FWFC_Block>& Blocks);
	bool Run(TMap<FIntVector, FWFC_Block>& OutGrid, const FIntVector& Size);
	bool IsInitialized() const { return !Blocks.IsEmpty(); }

private:
	bool Initialize(TMap<FIntVector, FWFC_Block>& OutGrid, const FIntVector& Size);
	bool CollapseCell(const FIntVector& Position, TMap<FIntVector, FWFC_Block>& OutGrid);
	void Propagate(const FIntVector& Position, TMap<FIntVector, FWFC_Block>& OutGrid);
	bool CheckCompatibility(const FWFC_Block& CollapsedBlock, const FWFC_Block& NeighborBlock, const FIntVector& Direction);
	bool FindLowestEntropyCell(FIntVector& OutPosition) const;

	int GetIndex(const FIntVector& Position) const;

	void BuildCompatibilityTable();

private:
	TArray<FWFC_Block> Blocks;
	FIntVector GridSize = FIntVector();
	TArray<TArray<FWFC_Block>> Wave; // 3D grid of block possibilities
	TArray<int> Entropies;          // Stores the entropy of each cell

	TMap<FIntVector, TMap<FWFC_Block, TArray<FWFC_Block>>> CompatibilityTable;

	FString ErrorMsg = FString();
};

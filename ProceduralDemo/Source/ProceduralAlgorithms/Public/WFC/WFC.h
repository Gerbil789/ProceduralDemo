#pragma once

#include "CoreMinimal.h"
#include "WFC_Block.h"

class PROCEDURALALGORITHMS_API WaveFunctionCollapse
{
public:
	WaveFunctionCollapse() = default;
	bool Run(const TArray<FWFC_Block>& Blocks, TMap<FIntVector, FWFC_Block>& OutGrid, const FIntVector& Size = FIntVector3(10, 10, 1));

private:
	void Initialize(const TArray<FWFC_Block>& Blocks, const FIntVector& Size, TMap<FIntVector, FWFC_Block>& OutGrid);
	bool CollapseCell(const FIntVector& Position, TMap<FIntVector, FWFC_Block>& OutGrid);
	void Propagate(const FIntVector& Position, TMap<FIntVector, FWFC_Block>& OutGrid);
	bool CheckCompatibility(const FWFC_Block& CollapsedBlock, const FWFC_Block& NeighborBlock, const FIntVector& Direction);
	bool FindFowestEntropyCell(FIntVector& OutPosition) const;

	int GetIndex(const FIntVector& Position) const;

private:
	FIntVector GridSize = FIntVector();
	TArray<TArray<FWFC_Block>> Wave; // 3D grid of block possibilities
	TArray<int> Entropies;          // Stores the entropy of each cell
};

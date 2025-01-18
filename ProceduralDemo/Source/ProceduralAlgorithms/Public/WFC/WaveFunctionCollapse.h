#pragma once

#include "CoreMinimal.h"
#include "Block.h"

namespace WFC
{
	class PROCEDURALALGORITHMS_API WaveFunctionCollapse
	{
	public:
		WaveFunctionCollapse() = default;
		bool Run(const TArray<FBlockWFC>& Blocks, TMap<FIntVector, FBlockWFC>& OutGrid, const FIntVector& Size = FIntVector3(10,10,1));

	private:
		void Initialize(const TArray<FBlockWFC>& Blocks, const FIntVector& Size);
		bool CollapseCell(const FIntVector& Position, TMap<FIntVector, FBlockWFC>& OutGrid);
		void Propagate(TArray<FIntVector>& PropagationStack, TMap<FIntVector, FBlockWFC>& OutGrid);
		bool CheckCompatibility(const FBlockWFC& CollapsedBlock, const FBlockWFC& NeighborBlock, const FIntVector& Direction);
		bool FindFowestEntropyCell(FIntVector& OutPosition) const;


		int GetIndex(const FIntVector& Position) const;

	private:
		FIntVector GridSize = FIntVector();
		TArray<TArray<FBlockWFC>> Wave; // 3D grid of block possibilities
		TArray<int> Entropies;          // Stores the entropy of each cell
	};

}


#pragma once

#include "CoreMinimal.h"
#include "WFC_Block.h"
#include "WaveFunctionCollapse.generated.h" 

UCLASS(Blueprintable)
class PROCEDURALALGORITHMS_API AWaveFunctionCollapse : public AActor
{
	GENERATED_BODY()

public:
	AWaveFunctionCollapse();

	UFUNCTION(BlueprintCallable, Category = "WFC")
	void SetBlocks(const TArray<FWFC_Block>& Blocks);

	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool Run();

	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool Initialize();

	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool FindLowestEntropyCell(FIntVector& OutPosition);

	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool CollapseCell(const FIntVector& Position);

	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool CollapseCellWithBlock(const FIntVector& Position, const FWFC_Block& Block);

	UFUNCTION(BlueprintCallable, Category = "WFC")
	void Propagate(const FIntVector& Position);

private:
	bool CheckCompatibility(const FWFC_Block& CollapsedBlock, const FWFC_Block& NeighborBlock, const FIntVector& Direction);
	void BuildCompatibilityTable();
	int GetIndex(const FIntVector& Position) const;
	int CalculateEntropy(const FIntVector& Position) const;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "WFC")
	TArray<FWFC_Block> Blocks;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	FIntVector GridSize = FIntVector(10, 10, 1);

	UPROPERTY(BlueprintReadWrite, Category = "WFC")
	TMap<FIntVector, FWFC_Block> Grid;

private:
	TArray<TArray<FWFC_Block>> Wave; // 3D grid of block possibilities
	TArray<int> Entropies;          // Stores the entropy of each cell
	TMap<FIntVector, TMap<FWFC_Block, TArray<FWFC_Block>>> CompatibilityTable;
};

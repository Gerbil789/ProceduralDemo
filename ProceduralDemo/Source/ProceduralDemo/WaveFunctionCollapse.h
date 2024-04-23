#pragma once

#include "CoreMinimal.h"
#include <stack>
#include <utility>
#include "WFCBlock.h"

class AWaveFunctionCollapse
{
public:	
    static TMap<FIntVector, WFCBlock> Generate(const TArray<WFCBlock>& blocks, const TMap<FIntVector, WFCBlock>& collapsedBlocks, const FIntVector& size);
private:
    static const TArray<FIntVector> Directions;
    static TArray<WFCBlock> Blocks;
    static FIntVector Size;
    static TMap<FIntVector, TArray<int>> Grid;
    static TMap<FIntVector, WFCBlock> CollapsedBlocks;
    
    static FIntVector GetLowestEntropyPosition();
    static void Collapse(const FIntVector& position);
    static void Propagate(const FIntVector& position);
    static void RemoveUnvalidBlocks(const FIntVector& position, const FIntVector& direction);
};

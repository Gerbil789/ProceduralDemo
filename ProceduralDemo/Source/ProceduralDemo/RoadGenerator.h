#pragma once

#include "WFCBlock.h"
#include "CoreMinimal.h"


class RoadGenerator
{
public:
	static TMap<FIntVector, WFCBlock> GenerateRoads(const TArray<WFCBlock>& allBlocks, const FIntVector& size);

};

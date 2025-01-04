#pragma once

#include "CoreMinimal.h"

class Utility
{
public:
	static FString CleanName(const FString& FullName);
	static bool IsInBounds(FIntVector position, FIntVector bounds);
};


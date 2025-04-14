#pragma once

#include "CoreMinimal.h"

class FProfilerUtils
{
public:
	static void InitSession();
	static void LogProfilingMessage(const FString& Message);

private:
	static FString SessionFilePath;
	static bool bInitialized;
};

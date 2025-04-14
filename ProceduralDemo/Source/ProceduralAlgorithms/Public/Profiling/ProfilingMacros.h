#pragma once

#include "CoreMinimal.h"
#include "ProfilerUtils.h" 

// === Global profiling toggle ===
#define ENABLE_PROFILING 1


#define ENABLE_TERRAIN_PROFILING 1

#if ENABLE_PROFILING && ENABLE_TERRAIN_PROFILING

#define TERRAIN_SCOPE_TIME(FUNC_NAME) \
	const double __StartTime = FPlatformTime::Seconds(); \
	ON_SCOPE_EXIT { \
		const double __Elapsed = FPlatformTime::Seconds() - __StartTime; \
		const FString __Message = FString::Printf(TEXT("%s took %.4f seconds"), TEXT(#FUNC_NAME), __Elapsed); \
		UE_LOG(LogTemp, Log, TEXT("%s"), *__Message); \
		FProfilerUtils::LogProfilingMessage(__Message); \
	};

#else

#define TERRAIN_SCOPE_TIME(FUNC_NAME)

#endif

#include "Profiling/ProfilerUtils.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"

FString FProfilerUtils::SessionFilePath;
bool FProfilerUtils::bInitialized = false;

void FProfilerUtils::InitSession()
{
	if (bInitialized) return;

	// Generate timestamped filename
	FString Timestamp = FDateTime::Now().ToString(TEXT("%Y-%m-%d_%H-%M-%S"));
	FString Directory = FPaths::ProjectSavedDir() / TEXT("Profiling");
	SessionFilePath = Directory / FString::Printf(TEXT("TerrainProfileLog_%s.txt"), *Timestamp);

	IFileManager::Get().MakeDirectory(*Directory, true);

	bInitialized = true;
}

void FProfilerUtils::LogProfilingMessage(const FString& Message)
{
	if (!bInitialized) InitSession();

	FFileHelper::SaveStringToFile(
		Message + LINE_TERMINATOR,
		*SessionFilePath,
		FFileHelper::EEncodingOptions::AutoDetect,
		&IFileManager::Get(),
		FILEWRITE_Append
	);
}

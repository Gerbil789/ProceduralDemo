#include "ProceduralWorldEditor.h"

void FProceduralDemoEditorModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("ProceduralDemoEditor loaded"));
}

void FProceduralDemoEditorModule::ShutdownModule()
{
	UE_LOG(LogTemp, Warning, TEXT("ProceduralDemoEditor shut down"));
}

IMPLEMENT_GAME_MODULE(FProceduralDemoEditorModule, ProceduralDemoEditor);

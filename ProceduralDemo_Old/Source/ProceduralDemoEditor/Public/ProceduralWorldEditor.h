#include "CoreUObject.h"
#include "Modules/ModuleManager.h"

class FProceduralDemoEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
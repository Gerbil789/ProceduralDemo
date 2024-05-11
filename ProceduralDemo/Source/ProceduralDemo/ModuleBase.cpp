#include "ModuleBase.h"
#include "ProceduralGridActor.h"

void AModuleBase::Process()
{

}

void AModuleBase::Initialize(TArray<int> blockIds, AProceduralGridActor* gridActor)
{
	GridActor = gridActor;
	BlockIds = blockIds;
}

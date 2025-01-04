#include "FillSidewalkModule.h"
#include "ProceduralGridActor.h"

void AFillSidewalkModule::Process()
{
	for (int x = 0; x < GridActor->Size.X; x++) {
		for (int y = 0; y < GridActor->Size.Y; y++) {
			if (GridActor->Grid.Contains(FIntVector(x, y, 0))) continue;
			GridActor->Grid.Add(FIntVector(x, y, 0), BlockIds[0]);
		}
	}
}

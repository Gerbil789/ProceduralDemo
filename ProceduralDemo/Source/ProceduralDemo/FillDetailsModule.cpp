#include "FillDetailsModule.h"
#include "ProceduralGridActor.h"

void AFillDetailsModule::Process()
{
	FString BlockName = GridActor->GetBlock(BlockIds[0]).Name;
	TArray<int> BlockIdsToFill = GridActor->FindBlocks(BlockName);

	for (auto& pair : GridActor->Grid) {
		if(BlockIdsToFill.Contains(pair.Value)){
			int i = 0;
			while (i++ < 20) {
				int blockId = BlockIds[FMath::RandRange(0, BlockIds.Num() - 1)];
				if (GridActor->GetBlock(blockId).Rotation == GridActor->GetBlock(pair.Value).Rotation)
				{
					UE_LOG(LogTemp, Warning, TEXT("Replacing a block"));
					pair.Value = blockId;
					break;
				}
			}
		}
	}
}

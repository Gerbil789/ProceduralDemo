#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Block.h"
#include "ModuleBase.generated.h"

class AProceduralGridActor; //forward declaration

USTRUCT(BlueprintType)
struct FBlockWithPriority
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABlockActor> Block;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 1;
};


UCLASS(Abstract)
class PROCEDURALDEMO_API AModuleBase : public AActor
{
	GENERATED_BODY()
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blocks")
	TArray<FBlockWithPriority> BlueprintBlocks; //input blocks

	void Initialize(TArray<int> blockIds, AProceduralGridActor* gridActor);
	virtual void Process();
protected:
	TArray<int> BlockIds;
	AProceduralGridActor* GridActor;
};

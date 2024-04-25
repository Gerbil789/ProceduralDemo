#pragma once

#include "CoreMinimal.h"
#include "ModuleBase.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "ProceduralGridActor.generated.h"


USTRUCT(BlueprintType)
struct FBlockWithPriority
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AWFC_Base> Block;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Priority = 1;
};

UCLASS()
class PROCEDURALDEMO_API AProceduralGridActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AProceduralGridActor();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FIntVector Size;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Delay = 0.01f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modules")
    TArray<TSubclassOf<AModuleBase>> ModuleClasses;

    UFUNCTION(BlueprintCallable)
    void GenerateMesh();

    UFUNCTION(BlueprintCallable)
    void CleanUpMesh();

    UFUNCTION(BlueprintCallable)
    void LoadBlocks();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blocks")
    TArray<FBlockWithPriority> BlueprintBlocks; 
private:
    TMap<FIntVector, WFCBlock> DeterminedBlocks;
    TArray<WFCBlock> AllBlocks;                     //all block types (with rotations)
    int Offset = 200;
   // FTimerHandle DelayHandle;
    TMap<int, UInstancedStaticMeshComponent*> InstancedMeshComponents = TMap<int, UInstancedStaticMeshComponent*>();


    void SpawnMesh(const FIntVector& position, const WFCBlock& block);
};

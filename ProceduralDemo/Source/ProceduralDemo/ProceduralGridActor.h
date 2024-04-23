#pragma once

#include "CoreMinimal.h"
#include "WaveFunctionCollapse.h"
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC")
    FIntVector Size;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC")
    float Delay = 0.01f;

    UFUNCTION(BlueprintCallable, Category = "WFC")
    void GenerateMesh();

    UFUNCTION(BlueprintCallable, Category = "WFC")
    void CleanUpMesh();

    UFUNCTION(BlueprintCallable, Category = "WFC_Blocks")
    void LoadBlocks();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC_Blocks")
    TArray<FBlockWithPriority> BlueprintBlocks; //block types (input)
private:
    TArray<WFCBlock> Blocks; //all block types (with rotations)
    int Offset = 200;
    FTimerHandle DelayHandle;
    TMap<int, UInstancedStaticMeshComponent*> InstancedMeshComponents = TMap<int, UInstancedStaticMeshComponent*>();
    void SpawnMesh(const FIntVector& position, const WFCBlock& block);
};

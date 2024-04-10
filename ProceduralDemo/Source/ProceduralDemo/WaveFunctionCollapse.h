#pragma once

#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include <stack>
#include <utility>// for std::pair
#include "WFC_Base.h"
#include "WFCBlock.h"
#include "GameFramework/Actor.h"
#include "WaveFunctionCollapse.generated.h"



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
class PROCEDURALDEMO_API AWaveFunctionCollapse : public AActor
{
	GENERATED_BODY()
	
public:	
	AWaveFunctionCollapse();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC")
    FIntVector Size;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC")
    float Delay = 0.2f;

    UFUNCTION(BlueprintCallable, Category = "WFC")
    void GenerateMesh();

    UFUNCTION(BlueprintCallable, Category = "WFC")
    void CleanUpMesh();

    UFUNCTION(BlueprintCallable, Category = "WFC_Blocks")
    void LoadBlocks();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC_Blocks")
    TArray<FBlockWithPriority> BlueprintBlocks;

private:
    static const TArray<FIntVector> Directions;
    TArray<WFCBlock> Blocks;
    int Offset = 200;
    FTimerHandle DelayHandle;
    int infiniteLoopBreaker = 20000; //TODO: remove this
    TMap<int, UInstancedStaticMeshComponent*> InstancedMeshComponents = TMap<int, UInstancedStaticMeshComponent*>();
    TMap<FIntVector, TArray<int>> IndexGrid = TMap<FIntVector, TArray<int>>();
    TMap<FIntVector, int> CollapsedBlocks = TMap<FIntVector, int>();

    std::stack<std::pair<FIntVector, int>> CalculatedBlocks;

    FIntVector GetLowestEntropyPosition();

    void Collapse(const FIntVector& position);

    void Propagate(const FIntVector& position);

    TSet<int> GetBlocksToRemove(const FIntVector& position, const FIntVector& direction);

    void SpawnBlocks();

    void SpawnMesh(const FIntVector& position, const int& blockIndex);
};

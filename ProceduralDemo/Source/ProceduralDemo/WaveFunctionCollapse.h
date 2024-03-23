#pragma once

#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "WFCBlock.h"
#include "GameFramework/Actor.h"
#include "WaveFunctionCollapse.generated.h"

USTRUCT()
struct FWFCBlockDataStruct : public FTableRowBase {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "MyDataTable")
    FString Up;

    UPROPERTY(EditAnywhere, Category = "MyDataTable")
    FString Down;

    UPROPERTY(EditAnywhere, Category = "MyDataTable")
    FString Left;

    UPROPERTY(EditAnywhere, Category = "MyDataTable")
    FString Right;

    UPROPERTY(EditAnywhere, Category = "MyDataTable")
    FString Front;

    UPROPERTY(EditAnywhere, Category = "MyDataTable")
    FString Back;

    UPROPERTY(EditAnywhere, Category = "MyDataTable")
    int Variants;
};


UCLASS()
class PROCEDURALDEMO_API AWaveFunctionCollapse : public AActor
{
	GENERATED_BODY()
	
public:	
	AWaveFunctionCollapse();

    //make datatable variable
    UPROPERTY(EditAnywhere, Category = DataTables)
    UDataTable* MyDataTable;


    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FIntVector Size;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Delay = 0.2f;

    UFUNCTION(BlueprintCallable, Category = "WFC")
    void LoadDataTable();

    UFUNCTION(BlueprintCallable, Category = "WFC")
    void GenerateMesh();

    UFUNCTION(BlueprintCallable, Category = "WFC")
    void CleanUpMesh();

    

private:
    TArray<WFCBlock> Blocks;
    int Offset = 200;
    FTimerHandle DelayHandle;
    int infiniteLoopBreaker = 1000; //TODO: remove this

    TMap<FString, UInstancedStaticMeshComponent*> InstancedMeshComponents = TMap<FString, UInstancedStaticMeshComponent*>();
    TMap<FIntVector, TArray<int>> IndexGrid = TMap<FIntVector, TArray<int>>();

    void WFC();

    void Propagate(const WFCBlock& block, const FIntVector& position);

    void SpawnMesh(const WFCBlock& block, const FIntVector& position);

    void RemoveInvalidBlocks(const WFCBlock& block, const FIntVector& position, const FString& direction);

    bool CompareSockets(const WFCBlock& block, const WFCBlock& neighborBlock, const FString& direction);
};

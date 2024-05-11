#pragma once

#include "CoreMinimal.h"
#include "ModuleBase.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "ProceduralGridActor.generated.h"

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<AModuleBase>> Modules;

    UFUNCTION(BlueprintCallable)
    void GenerateMesh();

    UFUNCTION(BlueprintCallable)
    void CleanUpMesh();

    UFUNCTION(BlueprintCallable)
    void LoadBlocks();

    Block& GetBlock(const int& index);

    TMap<FIntVector, int> Grid = TMap<FIntVector, int>(); //result grid
    TArray<Block> Blocks = TArray<Block>();   //all block types (with rotations)
private:
    int Offset = 200;
    FTimerHandle DelayHandle;
    TMap<int, UInstancedStaticMeshComponent*> InstancedMeshComponents = TMap<int, UInstancedStaticMeshComponent*>();

    void SpawnMesh();
    void SpawnBlock(const FIntVector& position, const int& blockIndex);
};

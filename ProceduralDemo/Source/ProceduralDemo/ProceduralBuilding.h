#pragma once

#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "ProceduralBuilding.generated.h"

UCLASS()
class PROCEDURALDEMO_API AProceduralBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	AProceduralBuilding();
	virtual ~AProceduralBuilding();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PCG_Building")
	FIntVector Size = FIntVector(10, 10, 10);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PCG_Building")
	int Offset = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PCG_Building")
	float Density = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PCG_Building")
	TArray<UStaticMesh*> WallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PCG_Building")
	TArray<UStaticMesh*> DoorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PCG_Building")
	TArray<UStaticMesh*> WindowMesh;

	UFUNCTION(BlueprintCallable, Category = "PCG_Building")
	void GenerateMesh();

	UFUNCTION(BlueprintCallable, Category = "PCG_Building")
	void CleanUpMesh();



private:
	TSet<FIntVector> BuildingBlocks;

	UInstancedStaticMeshComponent* WallInstancedMeshComponent;
	UInstancedStaticMeshComponent* DoorInstancedMeshComponent;
	UInstancedStaticMeshComponent* WindowInstancedMeshComponent;

	void SpawnBlock(FIntVector Position);
};

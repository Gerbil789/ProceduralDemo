#pragma once

#include "CoreMinimal.h"
#include "MarchingCubesAlgorithm.h"
#include "GameFramework/Actor.h"
#include "Chunk.generated.h"

UENUM()
enum class EMeshGenerationAlgorithm : uint8
{
	MarchingCubes = 0,
	OtherAlgorithm1 = 1,
	OtherAlgorithm2 = 2,
};

class UProceduralMeshComponent; //forward declaration


UCLASS()
class PROCEDURALDEMO_API AChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	AChunk();
	UPROPERTY(EditAnywhere, Category = "Chunk")
	UMaterialInterface* MeshMaterial;

	UFUNCTION(BlueprintCallable, Category = "Chunk")
	void GenerateMesh(const bool& smooth = false, const bool& normals = false);

	UFUNCTION(BlueprintCallable, Category = "Chunk")
	void CleanUpMesh();

	UProceduralMeshComponent* ProceduralMeshComponent = nullptr;
private:


};

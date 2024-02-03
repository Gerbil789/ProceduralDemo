#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarchingCubesActor.generated.h"

class UProceduralMeshComponent; //forward declaration

struct Cube
{
	FVector Points[8];
	float Values[8];
};


UCLASS()
class PROCEDURALDEMO_API AMarchingCubesActor : public AActor
{   
	GENERATED_BODY()
	
public:	
	AMarchingCubesActor();
	~AMarchingCubesActor();

	UPROPERTY(EditAnywhere, Category = "MarchingCubes")
	UMaterialInterface* MeshMaterial;

	UFUNCTION(BlueprintCallable, Category = "MarchingCubes")
	void GenerateMesh(const FIntVector& gridSize, const float& surfaceLevel = 0.5f, const float& offset = 100.0f, const bool& lerp = false);

	UFUNCTION(BlueprintCallable, Category = "MarchingCubes")
	void CleanUpMesh();

private:
	void GeneratePoints(const FIntVector& gridSize, float min = 0.0f, float max = 1.0f);
	void GenerateCubeMesh(const Cube& cube, const int32& cubeIndex, const bool& lerp);
	FVector InterpolateVertex(const FVector& pointA, const FVector& pointB, const float& valueA, const float& valueB);

	TMap<FVector, float> Points;
	UProceduralMeshComponent* ProceduralMeshComponent = nullptr;
	float SurfaceLevel;
	TArray<FVector> AllVertices;
	TArray<int32> AllTriangles;
	FVector ChunkOffset;
	float Offset;

	static const FVector CornerOffsets[8];

	static const int EdgeTable[256];

	static const int32 TriTable[256][16];
};

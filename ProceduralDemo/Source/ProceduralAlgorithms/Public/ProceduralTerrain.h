#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralTerrain.generated.h"

UCLASS()
class PROCEDURALALGORITHMS_API AProceduralTerrain : public AActor
{
	GENERATED_BODY()
	
public:	
	AProceduralTerrain();

protected:
  UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
  UProceduralMeshComponent* Mesh;

  UPROPERTY(EditAnywhere, Category = "Terrain")
  UMaterialInterface* TerrainMaterial;

  UPROPERTY(EditAnywhere, Category = "Terrain")
  int32 ChunkSize = 64; // Number of vertices in a chunk

  UPROPERTY(EditAnywhere, Category = "Terrain")
  int QuadSize = 100;

  UFUNCTION(BlueprintCallable)
  int32 GenerateChunk(int32 ChunkX, int32 ChunkY);
private:
  UPROPERTY(EditAnywhere, Category = "Terrain")
  float Frequency = 0.0005f;

  UPROPERTY(EditAnywhere, Category = "Terrain")
  float Amplitude = 450.0f;
};

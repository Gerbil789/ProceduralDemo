#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralTerrain.generated.h"


USTRUCT(BlueprintType)
struct FNoiseSettings
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  float Frequency = 0.5f;

  UPROPERTY(EditAnywhere)
  float Amplitude = 450.0f;

  UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0"))
  float Lacunarity = 2.0f; // How much the frequency changes per octave

  UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
  float Persistence = 0.5f; // How much the amplitude decreases per octave

  UPROPERTY(EditAnywhere)
  FVector2D Offset = FVector2D(0.0f, 0.0f);
};



UCLASS()
class PROCEDURALALGORITHMS_API AProceduralTerrain : public AActor
{
	GENERATED_BODY()
	
public:	
	AProceduralTerrain();

protected:
  UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Terrain")
  UProceduralMeshComponent* Mesh;

  UPROPERTY(EditAnywhere, Category = "Terrain")
	UMaterialInterface* TerrainMaterial;

  UPROPERTY(EditAnywhere, Category = "Terrain")
	int QuadSize = 100; // Size of a quad in Unreal units

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain|Chunks")
  int ChunkSize = 100;

  //UFUNCTION(BlueprintCallable, Category = "Terrain")
  //void GenerateChunkAsync(FIntPoint ChunkCoordinates, int32 ChunkSize);

  UFUNCTION(BlueprintCallable, Category = "Terrain")
  int32 GetChunkId(FIntPoint Chunk);

  UPROPERTY(EditAnywhere, Category = "Terrain")
  TArray<FNoiseSettings> NoiseOctaves;

	TMap<FIntPoint, int32> LoadedChunks;
	TSet<FIntPoint> ChunkIds;

  FCriticalSection CriticalSectionLock;
};

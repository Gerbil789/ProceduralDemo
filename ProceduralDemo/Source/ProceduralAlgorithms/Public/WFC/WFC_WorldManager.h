#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "WFC/WFC_Block.h"
#include "WFC/WFC_DataSet.h"
#include "WFC_WorldManager.generated.h"

//struct Chunk
//{
//  TMap<FIntVector, FWFC_Block> Grid;
//};

UCLASS()
class PROCEDURALALGORITHMS_API AWFC_WorldManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AWFC_WorldManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	APawn* PlayerPawn;
	FVector PlayerPosition;

  UPROPERTY(EditAnywhere, Category = "WFC")
  UWFC_DataSet* Dataset = nullptr;

  UPROPERTY(EditAnywhere, Category = "WFC")
  FIntVector GridSize = FIntVector(10, 10, 1);

  UPROPERTY(EditAnywhere, Category = "WFC")
  int Offset = 200;

  UPROPERTY(EditAnywhere, Category = "WFC")
  int32 LoadDistance = 2; 

  UPROPERTY(EditAnywhere, Category = "WFC")
  int32 UnloadDistance = 4;


  TSet<FIntVector> LoadedChunks; // Set of currently loaded chunks
  TMap<FIntVector, FWFC_Block> Grid; 
  TMap<UStaticMesh*, UInstancedStaticMeshComponent*> ISMComponents = TMap<UStaticMesh*, UInstancedStaticMeshComponent*>();

  bool LoadDataset();

  FIntVector GetChunkCoordinates(const FVector& Position) const;
  void UpdateChunks();
  void LoadChunk(const FIntVector& ChunkCoordinates);
  void UnloadChunk(const FIntVector& ChunkCoordinates);

};

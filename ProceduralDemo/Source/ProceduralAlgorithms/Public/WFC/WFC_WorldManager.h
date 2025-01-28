#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "WFC/WFC_Block.h"
#include "WFC/WFC_DataSet.h"
#include "WFC/WaveFunctionCollapse.h"
#include "WFC_WorldManager.generated.h"

UCLASS()
class PROCEDURALALGORITHMS_API AWFC_WorldManager : public AWaveFunctionCollapse
{
	GENERATED_BODY()
	
public:	
	AWFC_WorldManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	APawn* PlayerPawn = nullptr;
	FVector PlayerPosition = FVector();

  UPROPERTY(EditAnywhere, Category = "WFC")
  UWFC_DataSet* Dataset = nullptr;

  UPROPERTY(EditAnywhere, Category = "WFC")
  int Offset = 200;

  UPROPERTY(EditAnywhere, Category = "WFC")
  int32 LoadDistance = 2; 

  UPROPERTY(EditAnywhere, Category = "WFC")
  int32 UnloadDistance = 4;

  //UWaveFunctionCollapse WFC = UWaveFunctionCollapse();
  //UPROPERTY(BlueprintReadWrite, Category = "WFC")
  //UWaveFunctionCollapse* WFC;

  TSet<FIntVector> LoadedChunks; // Set of currently loaded chunks
  //TMap<FIntVector, FWFC_Block> Grid; 
  TMap<UStaticMesh*, UHierarchicalInstancedStaticMeshComponent*> HISMComponents;
  TQueue<FIntVector> ChunkGenerationQueue; // Queue for chunk generation
  bool bIsGeneratingChunk = false; // Flag to track if a chunk is being generated
  void ProcessNextChunk(); // Function to process the next chunk in the queue
  bool LoadDataset();

  FIntVector GetChunkCoordinates(const FVector& Position) const;
  void UpdateChunks();
  //void UnloadChunk(const FIntVector& ChunkCoordinates);
};
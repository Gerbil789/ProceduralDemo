#pragma once

#include "CoreMinimal.h"
#include "Chunk.h"
#include "GameFramework/Actor.h"
#include "InfiniteWorldGenerator.generated.h"



UCLASS()
class PROCEDURALDEMO_API AInfiniteWorldGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	AInfiniteWorldGenerator();
	~AInfiniteWorldGenerator();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InfiniteWorld")
	EMeshGenerationAlgorithm MeshGenerationAlgorithm = EMeshGenerationAlgorithm::MarchingCubes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InfiniteWorld")
	bool Smooth = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InfiniteWorld")
	bool Normals = false;

	UFUNCTION(BlueprintCallable, Category = "InfiniteWorld")
	void GenerateInitialChunks();

	UFUNCTION(BlueprintCallable, Category = "InfiniteWorld")
	void CleanUpChunks();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "InfiniteWorld")
	TSubclassOf<AChunk> ChunkActorBlueprint;

	UPROPERTY(EditAnywhere, Category = "InfiniteWorld")
	int LoadDistance = 2;

	UPROPERTY(EditAnywhere, Category = "InfiniteWorld")
	int UnloadDistance = 3;

	UPROPERTY(EditAnywhere, Category = "InfiniteWorld")
	float ChunkSize = 1000;

private:
	void UpdateWorld(const FVector& PlayerLocation);

	FIntVector CurrentChunkPosition;

	TMap<FIntVector, AChunk*> LoadedChunks;

	void LoadChunk(const FIntVector& ChunkLocation);

	void UnloadChunk(const FIntVector& ChunkLocation);

	FIntVector GetChunkPosition(const FVector& WorldPosition);
};

#include "InfiniteWorldGenerator.h"

AInfiniteWorldGenerator::AInfiniteWorldGenerator()
{
	PrimaryActorTick.bCanEverTick = true;

}


void AInfiniteWorldGenerator::BeginPlay()
{
	Super::BeginPlay();
}


void AInfiniteWorldGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	//UE_LOG(LogTemp, Warning, TEXT("PlayerLocation: %s"), *PlayerLocation.ToString());

}


void AInfiniteWorldGenerator::GenerateInitialMeshActors(const float& radius)
{
	FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	if (MarchingCubesActorBlueptint == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("MarchingCubesActorBlueptint is not set in the editor for %s"), *GetName());
		return;
	}

	FActorSpawnParameters SpawnParams;

	const int32 GridSizeX = 5;
	const int32 GridSizeY = 5;
	const int32 GridSizeZ = 2;
	const float ChunkSpacing = 5000.0f;

	for (int32 X = 0; X < GridSizeX; ++X)
	{
		for (int32 Y = 0; Y < GridSizeY; ++Y)
		{
			for (int32 Z = 0; Z < GridSizeZ; ++Z) {
				FVector SpawnLocation = FVector(X * ChunkSpacing, Y * ChunkSpacing, Z * GridSizeZ);

				AMarchingCubesActor* ChunkInstance = GetWorld()->SpawnActor<AMarchingCubesActor>(MarchingCubesActorBlueptint, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

				if (ChunkInstance)
				{
					ChunkInstance->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
					ChunkInstance->GenerateMesh(FIntVector(51, 51, 51), 0.48f, 100.0f, true);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to spawn ChunkInstance at location: %s"), *SpawnLocation.ToString());
				}
			}
		}
	}


}

void AInfiniteWorldGenerator::UpdateWorld(const FVector& PlayerLocation)
{


}

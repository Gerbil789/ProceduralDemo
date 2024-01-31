#include "InfiniteWorldGenerator.h"

AInfiniteWorldGenerator::AInfiniteWorldGenerator()
{
	PrimaryActorTick.bCanEverTick = true;

}

AInfiniteWorldGenerator::~AInfiniteWorldGenerator()
{
	CleanUpChunks();
	Super::Destroy();
}


void AInfiniteWorldGenerator::CleanUpChunks()
{
	ChunkActors.Empty();

	TArray<AActor*> ChildActors;
	this->GetAttachedActors(ChildActors);

	for (AActor* ChildActor : ChildActors) {
		if (ChildActor) {
			ChildActor->Destroy();
		}
	}
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


void AInfiniteWorldGenerator::GenerateInitialChunks(const float& radius, const FVector& playerLocation)
{
	//FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	CleanUpChunks();

	if (MarchingCubesActorBlueptint == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("MarchingCubesActorBlueptint is not set in the editor for %s"), *GetName());
		return;
	}

	FActorSpawnParameters SpawnParams;


	const float ChunkSpacing = 5000.0f;

	float distanceToPlayer;

	for (int32 X = -5; X < 5; ++X)
	{
		for (int32 Y = -5; Y < 5; ++Y)
		{
			for (int32 Z = -5; Z < 5; ++Z) {
				FVector SpawnLocation = FVector(X * ChunkSpacing, Y * ChunkSpacing, Z * ChunkSpacing);
				distanceToPlayer = FVector::Dist(playerLocation, SpawnLocation);

				if (distanceToPlayer > radius) {
					continue;
				}

				if (ChunkActors.Contains(SpawnLocation)) {
					continue;
				}

				AMarchingCubesActor* ChunkInstance = GetWorld()->SpawnActor<AMarchingCubesActor>(MarchingCubesActorBlueptint, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

				if (!ChunkInstance)
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to spawn ChunkInstance at location: %s"), *SpawnLocation.ToString());
			
				}
				UE_LOG(LogTemp, Warning, TEXT("Spawned ChunkInstance at location: %s"), *SpawnLocation.ToString());
				ChunkInstance->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
				ChunkInstance->GenerateMesh(FIntVector(51, 51, 51), 0.48f, 100.0f, true);
			}
		}
	}
}

void AInfiniteWorldGenerator::UpdateWorld(const FVector& PlayerLocation)
{


}

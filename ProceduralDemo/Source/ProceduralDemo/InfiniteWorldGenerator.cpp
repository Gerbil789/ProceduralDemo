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
	LoadedChunks.Empty();

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
	UpdateWorld(PlayerLocation);
}


void AInfiniteWorldGenerator::GenerateInitialChunks()
{
	if (ChunkActorBlueprint == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("MarchingCubesActorBlueptint is not set in the editor for %s"), *GetName());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("GenerateInitialChunks"));

	CurrentChunkPosition = FIntVector(99999, 99999, 99999); //make it something that will never be reached

	UpdateWorld(FVector(0,0,0));
	
}

void AInfiniteWorldGenerator::UpdateWorld(const FVector& PlayerLocation)
{
	FIntVector ChunkPosition = GetChunkPosition(PlayerLocation);
	if (ChunkPosition == CurrentChunkPosition)
	{
		return;
	}

	CurrentChunkPosition = ChunkPosition;
	UE_LOG(LogTemp, Warning, TEXT("CurrentChunkPosition: %s"), *CurrentChunkPosition.ToString());


	//load chunks
	for (int32 X = CurrentChunkPosition.X - LoadDistance; X <= CurrentChunkPosition.X + LoadDistance; ++X)
	{
		for (int32 Y = CurrentChunkPosition.Y - LoadDistance; Y <= CurrentChunkPosition.Y + LoadDistance; ++Y)
		{
			for (int32 Z = CurrentChunkPosition.Z - LoadDistance; Z <= CurrentChunkPosition.Z + LoadDistance; ++Z)
			{
				FIntVector ChunkLocation = FIntVector(X, Y, Z);

				if (LoadedChunks.Contains(ChunkLocation))
				{
					continue;
				}

				LoadChunk(ChunkLocation);
			}
		}	
	}

	//unload chunks
	for (auto& Elem : LoadedChunks)
	{
		FIntVector ChunkLocation = Elem.Key;
		if (FMath::Abs(ChunkLocation.X - CurrentChunkPosition.X) > UnloadDistance ||
			FMath::Abs(ChunkLocation.Y - CurrentChunkPosition.Y) > UnloadDistance ||
			FMath::Abs(ChunkLocation.Z - CurrentChunkPosition.Z) > UnloadDistance)
		{
			UnloadChunk(ChunkLocation);
		}
	}
}

void AInfiniteWorldGenerator::LoadChunk(const FIntVector& ChunkLocation)
{
	FVector SpawnLocation = FVector(ChunkLocation.X, ChunkLocation.Y, ChunkLocation.Z) * ChunkSize + ChunkSize/2;
	AChunk* ChunkInstance = GetWorld()->SpawnActor<AChunk>(ChunkActorBlueprint, SpawnLocation, FRotator::ZeroRotator);
	if (!ChunkInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn ChunkInstance at location: %s"), *SpawnLocation.ToString());
		return;
	}
	ChunkInstance->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

	switch (MeshGenerationAlgorithm)
	{
	case EMeshGenerationAlgorithm::MarchingCubes:
		//ChunkInstance->GenerateMesh(FIntVector(11, 11, 11), 0.5f, 500.0f, true);
		ChunkInstance->GenerateMesh(Smooth, Normals);
		break;
	case EMeshGenerationAlgorithm::OtherAlgorithm1:
		UE_LOG(LogTemp, Error, TEXT("OtherAlgorithm1 is not implemented yet"));
		break;
	case EMeshGenerationAlgorithm::OtherAlgorithm2:
		UE_LOG(LogTemp, Error, TEXT("OtherAlgorithm2 is not implemented yet"));
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("Invalid MeshGenerationAlgorithm for %s"), *GetName());
		return;
	}


	LoadedChunks.Add(ChunkLocation, ChunkInstance);
}

void AInfiniteWorldGenerator::UnloadChunk(const FIntVector& ChunkLocation)
{
	AChunk* ChunkInstance = LoadedChunks[ChunkLocation];
	if (!ChunkInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("ChunkInstance is null for location: %s"), *ChunkLocation.ToString());
		return;
	}
	ChunkInstance->MarkAsGarbage();
	ChunkInstance->Destroy();
	LoadedChunks.Remove(ChunkLocation);
}

FIntVector AInfiniteWorldGenerator::GetChunkPosition(const FVector& WorldPosition)
{
	return FIntVector(
		FMath::FloorToInt(WorldPosition.X / ChunkSize),
		FMath::FloorToInt(WorldPosition.Y / ChunkSize),
		FMath::FloorToInt(WorldPosition.Z / ChunkSize)
	);

}

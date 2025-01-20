#include "WFC/WFC_WorldManager.h"
#include "WFC/WFC.h"
#include "Kismet/GameplayStatics.h"

AWFC_WorldManager::AWFC_WorldManager()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComponent);
}

void AWFC_WorldManager::BeginPlay()
{
	Super::BeginPlay();

	// Automatically find the player character
	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!PlayerPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("Player not found!"));
	}

	if (!Dataset)
	{
		UE_LOG(LogTemp, Error, TEXT("No dataset assigned."));
		return;
	}

	if(!LoadDataset())
  {
		UE_LOG(LogTemp, Error, TEXT("Failed to load dataset."));
  }
}

void AWFC_WorldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerPawn)
	{
		return;
	}

	if (!Dataset)
	{
		return;
	}

	PlayerPosition = PlayerPawn->GetActorLocation();
	UpdateChunks();
}

bool AWFC_WorldManager::LoadDataset()
{
  if (!Dataset)
  {
    UE_LOG(LogTemp, Error, TEXT("No data set assigned."));
    return false;
  }

  if (Dataset->Blocks.Num() == 0)
  {
    UE_LOG(LogTemp, Error, TEXT("No blocks in data set."));
    return false;
  }

  // Clear the InstancedStaticMeshComponents
  for (auto& Pair : ISMComponents)
  {
    if (Pair.Value)
    {
      Pair.Value->DestroyComponent();
    }
  }
  ISMComponents.Empty();

  // Create InstancedStaticMeshComponent for each unique static mesh
  for (const FWFC_Block& Block : Dataset->Blocks)
  {
    if (!Block.StaticMesh && !Block.IsEmpty && !Block.IsFill)
    {
      UE_LOG(LogTemp, Error, TEXT("Block does not have a valid StaticMesh."));
      return false;
    }

    if (ISMComponents.Contains(Block.StaticMesh))
    {
      continue;	// No need for duplicate ISM components
    }

    FName ComponentName = FName(Block.ToString());
    UInstancedStaticMeshComponent* ISMComp = NewObject<UInstancedStaticMeshComponent>(this, ComponentName);

    if (!ISMComp)
    {
      UE_LOG(LogTemp, Error, TEXT("Failed to create InstancedStaticMeshComponent for block."));
      return false;
    }

    ISMComp->SetupAttachment(RootComponent, ComponentName);
    ISMComp->SetStaticMesh(Block.StaticMesh);
    ISMComp->bUseDefaultCollision = true;
    ISMComp->RegisterComponent();

    ISMComponents.Add(Block.StaticMesh, ISMComp);

    UE_LOG(LogTemp, Display, TEXT("Created and registered InstancedStaticMeshComponent for mesh: %s"), *Block.ToString());
  }

  return true;
}

FIntVector AWFC_WorldManager::GetChunkCoordinates(const FVector& Position) const
{
  return FIntVector(FMath::FloorToInt(Position.X / (GridSize.X * Offset)), FMath::FloorToInt(Position.Y / (GridSize.Y * Offset)), 0);
}

void AWFC_WorldManager::UpdateChunks()
{
  FIntVector PlayerChunk = GetChunkCoordinates(PlayerPosition);

  // Unload chunks that are too far away
  //TArray<FIntVector> ChunksToUnload;
  //for (const FIntVector& Chunk : LoadedChunks)
  //{
  //  if (FMath::Abs(Chunk.X - PlayerChunk.X) > UnloadDistance || FMath::Abs(Chunk.Y - PlayerChunk.Y) > UnloadDistance)
  //  {
  //    ChunksToUnload.Add(Chunk);
  //  }
  //}
  //for (const FIntVector& Chunk : ChunksToUnload)
  //{
  //  UnloadChunk(Chunk);
  //}

  // Load chunks within range
  for (int32 X = PlayerChunk.X - LoadDistance; X <= PlayerChunk.X + LoadDistance; ++X)
  {
    for (int32 Y = PlayerChunk.Y - LoadDistance; Y <= PlayerChunk.Y + LoadDistance; ++Y)
    {
      FIntVector ChunkCoordinates(X, Y, 0);
      if (!LoadedChunks.Contains(ChunkCoordinates))
      {
        LoadChunk(ChunkCoordinates);
      }
    }
  }
}

void AWFC_WorldManager::LoadChunk(const FIntVector& ChunkCoordinates)
{
	FIntVector GridCoordinates = FIntVector(ChunkCoordinates.X * GridSize.X, ChunkCoordinates.Y * GridSize.Y, 0);
  TMap<FIntVector, FWFC_Block> ExpandedGrid;
	ExpandedGrid.Reserve((GridSize.X + 2) * (GridSize.Y + 2) * GridSize.Z);

	for (int x = -1; x <= GridSize.X; x++)
	{
		for (int y = -1; y <= GridSize.Y; y++)
		{
			for (int z = 0; z < GridSize.Z; z++)
			{
				if (x == -1 || y == -1 || x == GridSize.X || y == GridSize.Y)
				{
					FIntVector Position = FIntVector(x, y, z) + GridCoordinates;

					if (Grid.Contains(Position))
					{
						ExpandedGrid.Add(FIntVector(x + 1, y + 1, 0), Grid[Position]);
					}
				}
			}
		}
	}

	WaveFunctionCollapse WFC;
	FIntVector ExpandedGridSize = FIntVector(GridSize.X + 2, GridSize.Y + 2, GridSize.Z);
	if (!WFC.Run(Dataset->Blocks, ExpandedGrid, ExpandedGridSize))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate grid for chunk: %s"), *ChunkCoordinates.ToString());
		return;
	}

	if (ExpandedGrid.Num() != (ExpandedGridSize.X * ExpandedGridSize.Y * ExpandedGridSize.Z))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid grid size for chunk: %s"), *ChunkCoordinates.ToString());
		return;
	}

	// trim expanded grid
	TMap<FIntVector, FWFC_Block> NewGridData;
	NewGridData.Reserve(GridSize.X * GridSize.Y * GridSize.Z);
	for (int x = 1; x <= GridSize.X; x++)
	{
		for (int y = 1; y <= GridSize.Y; y++)
		{
			for (int z = 0; z < GridSize.Z; z++)
			{
				FWFC_Block Block = ExpandedGrid[FIntVector(x, y, z)];
				FIntVector Position = GridCoordinates + FIntVector(x - 1, y - 1, z);
				NewGridData.Add(Position, Block);
				Grid.Add(Position, Block);
			}
		}
	}

	for (auto& Pair : NewGridData)
	{
		FIntVector Position = Pair.Key;
		FWFC_Block Block = Pair.Value;
		if (ISMComponents.Contains(Block.StaticMesh))
		{
			UInstancedStaticMeshComponent* ISMComp = ISMComponents[Block.StaticMesh];
			ISMComp->AddInstance(FTransform(FRotator(0, Block.Rotation, 0), FVector(Position * Offset)));
		}
	}

	LoadedChunks.Add(ChunkCoordinates);
	UE_LOG(LogTemp, Log, TEXT("Loaded Chunk: %s"), *ChunkCoordinates.ToString());
}

void AWFC_WorldManager::UnloadChunk(const FIntVector& ChunkCoordinates)
{
	//if (!LoadedChunks.Contains(ChunkCoordinates))
	//{
	//	return;
	//}

	//Chunk& Chunk = Chunks[ChunkCoordinates];


	//for (auto& Pair : Chunk.Grid)
	//{
	//	FIntVector Position = Pair.Key;
	//	FWFC_Block Block = Pair.Value;

	//	if (Block.IsEmpty || Block.IsFill)
	//	{
	//		continue;
	//	}

	//	if (!ISMComponents.Contains(Block.StaticMesh))
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("UnloadChunk: Block does not have a valid StaticMesh."));
	//		continue;
	//	}

	//	UInstancedStaticMeshComponent* ISMComp = ISMComponents[Block.StaticMesh];

	//	if (!ISMComp)
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("UnloadChunk: Invalid InstancedStaticMeshComponent."));
	//		continue;
	//	}

	//	//ISMComp->RemoveInstance();

	//}
	//LoadedChunks.Remove(ChunkCoordinates);
	//Chunks.Remove(ChunkCoordinates);
	//UE_LOG(LogTemp, Log, TEXT("Unloaded Chunk: %s"), *ChunkCoordinates.ToString());

}
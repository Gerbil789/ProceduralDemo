#include "WFC/GridActor.h"
#include "WFC/Utility.h"
#include "WFC/WaveFunctionCollapse.h"

AGridActor::AGridActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComponent);
}

void AGridActor::LoadMeshes()
{
	// Load blocks from directory
	if(WFC::Utility::LoadBlocksFromDirectory(MeshDirectoryPath, Blocks, bAddEmptyBlock, bAddFillBlock) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load blocks from directory: %s"), *MeshDirectoryPath);
		return;
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

	// Create InstancedStaticMeshComponents for each static mesh
	for (const FBlockWFC& Block : Blocks)
	{
		if (!Block.StaticMesh && !Block.IsEmpty && !Block.IsFill)
		{
			UE_LOG(LogTemp, Error, TEXT("Block does not have a valid StaticMesh."));
			continue;
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
			continue;
		}

		ISMComp->SetupAttachment(RootComponent, ComponentName);
		ISMComp->SetStaticMesh(Block.StaticMesh);
		ISMComp->bUseDefaultCollision = true;
		ISMComp->RegisterComponent();

		ISMComponents.Add(Block.StaticMesh, ISMComp);

		UE_LOG(LogTemp, Display, TEXT("Created and registered InstancedStaticMeshComponent for mesh: %s"), *Block.ToString());
	}
}

void AGridActor::GenerateMesh()
{
	CleanUpMesh();

	if (Blocks.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No blocks loaded."));
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("No blocks loaded."), FText::FromString(__FUNCTION__));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("----------------------- GENERATING MESH -----------------------"));

	WFC::WaveFunctionCollapse WFC_Algorithm;
	if (!WFC_Algorithm.Run(Blocks, Grid, GridSize))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate grid."));
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Failed to generate grid."), FText::FromString(__FUNCTION__));
		return;
	}

	if (Grid.Num() == 0) 
	{
		UE_LOG(LogTemp, Error, TEXT("No cells generated."));
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("No cells generated."), FText::FromString(__FUNCTION__));
		return;
	}

	int TotalCells = GridSize.X * GridSize.Y * GridSize.Z;
	if(Grid.Num() != TotalCells)
	{
		//The reason for this is very likely bad input data
		UE_LOG(LogTemp, Error, TEXT("Failed to generate all cells. Generated %d/%d cells."), Grid.Num(), TotalCells);
	}

	if (!Render())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to render grid."));
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Failed to render grid."), FText::FromString(__FUNCTION__));
	}
}

void AGridActor::CleanUpMesh()
{
	UE_LOG(LogTemp, Warning, TEXT("Cleaning up"));

	for(auto& Pair : ISMComponents)
	{
		if (Pair.Value)
		{
			Pair.Value->ClearInstances();
		}
	}
	Grid.Empty();
}

bool AGridActor::Render()
{
	const FVector GridCenter = FVector((GridSize.X - 1) * Offset / 2.0f, (GridSize.Y - 1) * Offset / 2.0f, 0.0f);

	for (auto& Pair : Grid)
	{
		const FBlockWFC& Block = Pair.Value;
		const FIntVector& Position = Pair.Key;

		UInstancedStaticMeshComponent** ISMCompPtr = ISMComponents.Find(Block.StaticMesh);
		if (!ISMCompPtr || !(*ISMCompPtr))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to find InstancedStaticMeshComponent for block."));
			return false;
		}

		UInstancedStaticMeshComponent* ISMComp = *ISMCompPtr;
		FVector Location = (FVector)Position * Offset - GridCenter;
		FRotator Rotation = FRotator(0, Block.Rotation, 0);
		FTransform InstanceTransform(Rotation, Location);
		ISMComp->AddInstance(InstanceTransform);
	}

	return true;
}
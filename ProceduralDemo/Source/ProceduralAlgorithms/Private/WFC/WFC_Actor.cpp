#include "WFC/WFC_Actor.h"
#include "WFC/WFC_Utility.h"
#include "WFC/WFC.h"

AWFC_Actor::AWFC_Actor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComponent);
}

// Reload dataset when changed
void AWFC_Actor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Get the name of the changed property
	FName PropertyName = (PropertyChangedEvent.Property != nullptr)
		? PropertyChangedEvent.Property->GetFName()
		: NAME_None;

	// Check if the changed property is the "Dataset" property
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AWFC_Actor, Dataset))
	{
		// Call your custom function or handle the change
		UE_LOG(LogTemp, Log, TEXT("Dataset property changed!"));
		LoadDataset();
	}
}

bool AWFC_Actor::LoadDataset()
{
	if(!Dataset)
	{
		UE_LOG(LogTemp, Error, TEXT("No data set assigned."));
		return false;
	}

	if (Dataset->Blocks.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No blocks in data set."));
		return false;
	}

	Blocks = Dataset->Blocks;

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
	for (const FWFC_Block& Block : Blocks)
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

void AWFC_Actor::GenerateMesh()
{
	CleanUpMesh();

	if (Blocks.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No blocks loaded. Loading..."));
		if(!LoadDataset())
		{
			UE_LOG(LogTemp, Error, TEXT("No blocks loaded."));
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("No blocks loaded."), FText::FromString(__FUNCTION__));
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("----------------------- GENERATING MESH -----------------------"));

	// TEST
	//for(int x = 0; x < GridSize.X; x++)
	//{
	//	for (int y = 0; y < GridSize.Y; y++)
	//	{
	//		if (x == 0 || y == 0 || x == GridSize.X - 1 || y == GridSize.Y - 1)
	//		{
	//			Grid.Add(FIntVector(x, y, 0), FWFC_Block(Blocks[0]));
	//		}
	//	}
	//}

	//for (int x = 0; x < GridSize.X; x++)
	//{
	//	Grid.Add(FIntVector(x, GridSize.Y, 0), FWFC_Block(Blocks[0]));
	//}

	//FIntVector TmpSize = GridSize;
	//TmpSize.Y++;

	WaveFunctionCollapse WFC_Algorithm;
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
	if(Grid.Num() < TotalCells)
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

void AWFC_Actor::CleanUpMesh()
{
	UE_LOG(LogTemp, Warning, TEXT("Cleaning up"));

	// Clear the InstancedStaticMeshComponents
	for(auto& Pair : ISMComponents)
	{
		if (Pair.Value)
		{
			Pair.Value->ClearInstances();
		}
	}

	// Clear the grid
	Grid.Empty();
}

bool AWFC_Actor::Render()
{
	const FVector GridCenter = FVector((GridSize.X - 1) * Offset / 2.0f, (GridSize.Y - 1) * Offset / 2.0f, 0.0f);

	for (auto& Pair : Grid)
	{
		const FWFC_Block& Block = Pair.Value;
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
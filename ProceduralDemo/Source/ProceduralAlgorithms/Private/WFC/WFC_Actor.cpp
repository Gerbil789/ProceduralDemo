#include "WFC/WFC_Actor.h"
#include "WFC/WFC_Utility.h"


AWFC_Actor::AWFC_Actor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComponent);
}

void AWFC_Actor::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
#if WITH_EDITOR
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
#endif
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AWFC_Actor, Dataset))
	{
		UE_LOG(LogTemp, Log, TEXT("Dataset property changed!"));
		LoadDataset();
	}
}

void AWFC_Actor::LoadDataset_Implementation()
{
	if(!Dataset)
	{
		UE_LOG(LogTemp, Error, TEXT("No data set assigned."));
		return;
	}

	if (Dataset->Modules.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No blocks in data set."));
		return;
	}

	AWaveFunctionCollapse::SetModules(Dataset->Modules);

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
	for (const FWFC_Module& Block : Dataset->Modules)
	{
		if (!Block.StaticMesh && !Block.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("Block does not have a valid StaticMesh."));
			return;
		}

		if (ISMComponents.Contains(Block.StaticMesh))
		{
			continue;	// No need for duplicate ISM components
		}

		if (!Debug)
		{
			// dont render empty blocks & fill blocks
			FString Name = Block.StaticMesh->GetName();
			if (Name == "1s_1s_1s_1s_1i_1i_1" || Name == "0s_0s_0s_0s_0i_0i_1")
			{
				continue;
			}
		}

		FName ComponentName = FName(Block.ToString());
		UInstancedStaticMeshComponent* ISMComp = NewObject<UInstancedStaticMeshComponent>(this, ComponentName);

		if (!ISMComp)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create InstancedStaticMeshComponent for block."));
			return;
		}

		ISMComp->SetupAttachment(RootComponent, ComponentName);
		ISMComp->SetStaticMesh(Block.StaticMesh);
		ISMComp->bUseDefaultCollision = true;
		ISMComp->RegisterComponent();

		ISMComponents.Add(Block.StaticMesh, ISMComp);
	}

	UE_LOG(LogTemp, Display, TEXT("Loaded %d blocks."), Dataset->Modules.Num());
}

void AWFC_Actor::GenerateMesh_Implementation()
{
	CleanUpMesh();

	if (!RunWFC())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to run WFC."));
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Failed to run WFC."), FText::FromString(__FUNCTION__));
		return;
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
	for(auto& Pair : ISMComponents)
	{
		if (Pair.Value)
		{
			Pair.Value->ClearInstances();
		}
	}
	Grid.Empty();
}

bool AWFC_Actor::RunWFC()
{
	/*if (!Grid.IsEmpty())
	{
		int newBlocks = 0;
		for (auto& Pair : Grid)
		{
			const FWFC_Module& Block = Pair.Value;
			const FIntVector& Position = Pair.Key;

			if(!Dataset->Modules.Contains(Block))
			{
				Dataset->Modules.Add(Block);
				newBlocks++;
			}
		}
		if (newBlocks > 0)
		{
			LoadDataset();
		}
	}*/

	if (Modules.IsEmpty())
	{
		LoadDataset();
	}

	if (!AWaveFunctionCollapse::Run())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate grid."));
		return false;
	}

	int TotalCells = GridSize.X * GridSize.Y * GridSize.Z;
	if (Grid.Num() < TotalCells)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate all cells. Generated %d/%d cells."), Grid.Num(), TotalCells);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Generated %d/%d cells."), Grid.Num(), TotalCells);
	}

	return true;
}

bool AWFC_Actor::Render()
{
	const FVector GridCenter = FVector((GridSize.X - 1) * Offset / 2.0f, (GridSize.Y - 1) * Offset / 2.0f, 0.0f);

	for (auto& Pair : Grid)
	{
		const FWFC_Module& Block = Pair.Value;
		const FIntVector& Position = Pair.Key;

		UInstancedStaticMeshComponent** ISMCompPtr = ISMComponents.Find(Block.StaticMesh);
		if (!ISMCompPtr || !(*ISMCompPtr))
		{
			//UE_LOG(LogTemp, Error, TEXT("Failed to find InstancedStaticMeshComponent for block."));
			//return false;

			continue;
		}

		UInstancedStaticMeshComponent* ISMComp = *ISMCompPtr;
		FVector Location = (FVector)Position * Offset - GridCenter;
		FRotator Rotation = FRotator(0, Block.Rotation, 0);
		ISMComp->AddInstance(FTransform(Rotation, Location));
	}

	return true;
}
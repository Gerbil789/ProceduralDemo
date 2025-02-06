#include "WFC/WFC_DemoActor.h"
#include "Editor.h"
#include "TimerManager.h"

AWFC_DemoActor::AWFC_DemoActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComponent);

}

void AWFC_DemoActor::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
#if WITH_EDITOR
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
#endif
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AWFC_DemoActor, Dataset))
	{
		UE_LOG(LogTemp, Log, TEXT("Dataset property changed!"));
		LoadDataset();
	}
}

void AWFC_DemoActor::Start()
{
	CleanUpMesh();
	Grid.Empty();

	//clear text components
	for (auto& Pair : TextComponents)
	{
		if (Pair.Value)
		{
			Pair.Value->DestroyComponent();
		}
	}
	TextComponents.Empty();

	if(!Initialize())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to initialize"));
		return;
	}

	const FVector GridCenter = FVector((GridSize.X - 1) * Offset / 2.0f, (GridSize.Y - 1) * Offset / 2.0f, 0.0f);

	// set text compoents
	for (int x = 0; x < GridSize.X; x++)
	{
		for (int y = 0; y < GridSize.Y; y++)
		{
			for (int z = 0; z < GridSize.Z; z++)
			{
				FIntVector GridPosition = FIntVector(x, y, z);
				FVector Position = (FVector)GridPosition * Offset - GridCenter;
				UTextRenderComponent* TextComponent = NewObject<UTextRenderComponent>(this);
				TextComponent->SetupAttachment(RootComponent);
				TextComponent->RegisterComponent();
				TextComponent->SetRelativeLocation(Position);

				TextComponent->SetWorldSize(50.0f);           // Ensure text is large enough
				TextComponent->SetHorizontalAlignment(EHTA_Center);
				TextComponent->SetVerticalAlignment(EVRTA_TextCenter);
				TextComponent->SetTextRenderColor(FColor::Green);

				int Index = GetIndex(GridPosition);
				FString Text = FString::Printf(TEXT("%.2f"), Wave[Index].Entropy);


				TextComponent->SetText(FText::FromString(Text));
				TextComponents.Add(GridPosition, TextComponent);
			}
		}
	}


	UE_LOG(LogTemp, Log, TEXT("Initialized"));
}

void AWFC_DemoActor::Next()
{
	Iterate();
}

void AWFC_DemoActor::Finish()
{
	if (!bIsIterating)
	{
		bIsIterating = true;
		IterateWithDelay(); // Start the first iteration
	}
	else
	{
		bIsIterating = false;
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(IterationTimerHandle);
	}
}

void AWFC_DemoActor::LoadDataset()
{
	if (!Dataset)
	{
		UE_LOG(LogTemp, Error, TEXT("No data set assigned."));
		return;
	}

	AWaveFunctionCollapse::SetModules(Dataset->Modules);

	for (auto& Pair : ISMComponents)
	{
		if (Pair.Value)
		{
			Pair.Value->DestroyComponent();
		}
	}
	ISMComponents.Empty();

	for (const FWFC_Module& Block : Dataset->Modules)
	{
		if (!Block.StaticMesh)
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
			if (!Block.StaticMesh)
			{
				UE_LOG(LogTemp, Error, TEXT("Block does not have a valid StaticMesh."));
				return;
			}

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

void AWFC_DemoActor::CleanUpMesh()
{
	UE_LOG(LogTemp, Warning, TEXT("Cleaning up"));
	for (auto& Pair : ISMComponents)
	{
		if (Pair.Value)
		{
			Pair.Value->ClearInstances();
		}
	}


}

bool AWFC_DemoActor::Iterate()
{
	FIntVector LowestEntropySlot;
	if (!FindLowestEntropySlot(LowestEntropySlot))
	{
		UE_LOG(LogTemp, Error, TEXT("No valid slot found"));
		return false; // We have collapsed all cells
	}

	if (!CollapseSlot(LowestEntropySlot))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to collapse slot"));
		return false; // No valid modules to collapse to
	}

	UE_LOG(LogTemp, Log, TEXT("Collapsed slot: %s"), *LowestEntropySlot.ToString());

	Propagate(LowestEntropySlot);

	//log grid count
	UE_LOG(LogTemp, Log, TEXT("Grid count: %d/%d"), Grid.Num(), GridSize.X * GridSize.Y * GridSize.Z);


	//render
	const FVector GridCenter = FVector((GridSize.X - 1) * Offset / 2.0f, (GridSize.Y - 1) * Offset / 2.0f, 0.0f);
	const FWFC_Module Block = Grid[LowestEntropySlot];

	UInstancedStaticMeshComponent** ISMCompPtr = ISMComponents.Find(Block.StaticMesh);
	if (!ISMCompPtr || !(*ISMCompPtr))
	{
		return true;
	}

	UInstancedStaticMeshComponent* ISMComp = *ISMCompPtr;
	FVector Location = (FVector)LowestEntropySlot * Offset - GridCenter;
	FRotator Rotation = FRotator(0, Block.Rotation, 0);
	ISMComp->AddInstance(FTransform(Rotation, Location));


	// Update all text components

	for (auto& Pair : TextComponents)
	{
		if (Grid.Contains(Pair.Key))
		{
			//delete component
			Pair.Value->DestroyComponent();
			continue;
		}

		//update component text to current entropy
		int Index = GetIndex(Pair.Key);
		FString Text = FString::Printf(TEXT("%.2f"), Wave[Index].Entropy);
		Pair.Value->SetText(FText::FromString(Text));


	}


	return true;
}

void AWFC_DemoActor::IterateWithDelay()
{
	if (!Iterate())
	{
		bIsIterating = false;
		UE_LOG(LogTemp, Log, TEXT("Finished"));
		return;
	}

	// Schedule the next iteration with a delay
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(
		IterationTimerHandle,
		this,
		&AWFC_DemoActor::IterateWithDelay,
		DelayDuration,
		false // Do not loop
	);
}
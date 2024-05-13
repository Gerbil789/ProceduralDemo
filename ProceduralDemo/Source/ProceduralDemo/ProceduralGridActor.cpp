#include "ProceduralGridActor.h"
#include "Utility.h"

AProceduralGridActor::AProceduralGridActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void AProceduralGridActor::LoadBlocks()
{
	CleanUpMesh();
	UE_LOG(LogTemp, Warning, TEXT("Loading blocks..."));
	Blocks = TArray<Block>();
	int meshId = 1;

	for (auto module : Modules)
	{
		AModuleBase& Module = *module->GetDefaultObject<AModuleBase>();
		FString ModuleName = Utility::CleanName(*Module.GetName()); //clean module name from engine garbage
		UE_LOG(LogTemp, Warning, TEXT("Loading blocks from %s"), *ModuleName);
		if (Module.BlueprintBlocks.Num() == 0) UE_LOG(LogTemp, Error, TEXT("No blocks found in %s"), *ModuleName);

		TArray<int> ModuleIndices;


		for (auto block : Module.BlueprintBlocks)
		{
			if (block.Block == nullptr) UE_LOG(LogTemp, Error, TEXT("Empty block in %s"), *ModuleName);

			ABlockActor& blockActor = *block.Block->GetDefaultObject<ABlockActor>();

			if (blockActor.Mesh == nullptr) {
				int index = Blocks.AddUnique(Block(0, 0, block.Priority, blockActor));
				ModuleIndices.Add(index);
				continue;
			}

			for (int rotation = 0; rotation < blockActor.Variants; rotation++)
			{
				int angle = rotation * -90;
				int index = Blocks.AddUnique(Block(meshId, angle, block.Priority, blockActor));
				ModuleIndices.Add(index);
			}

			//create instanced mesh component
			UInstancedStaticMeshComponent* NewInstancedMeshComponent = NewObject<UInstancedStaticMeshComponent>(this);
			NewInstancedMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			NewInstancedMeshComponent->SetStaticMesh(blockActor.Mesh);
			NewInstancedMeshComponent->bUseDefaultCollision = true;
			NewInstancedMeshComponent->SetVisibility(true, true);
			InstancedMeshComponents.Add(meshId, NewInstancedMeshComponent);

			meshId++;
		}

		Module.Initialize(ModuleIndices, this);

	}
	UE_LOG(LogTemp, Warning, TEXT("Loaded blocks count: %d"), Blocks.Num());
}

Block& AProceduralGridActor::GetBlock(const int& index)
{
	try
	{
		return Blocks[index];
	}
	catch (const std::exception& e)
	{
		UE_LOG(LogTemp, Error, TEXT("Exception: %s"), UTF8_TO_TCHAR(e.what()));
		return Blocks[0];
	}
}

int AProceduralGridActor::FindBlockIndex(const FString& name)
{
	for (int i = 0; i < Blocks.Num(); i++)
	{
		if (Blocks[i].Name == name) return i;
	}

	FString message = FString::Printf(TEXT("Block %s not found"), *name);
	throw std::runtime_error(TCHAR_TO_UTF8(*message));
}

void AProceduralGridActor::CleanUpMesh()
{
	UE_LOG(LogTemp, Warning, TEXT("Cleaning up"));

	for (auto MeshComponent : InstancedMeshComponents)
	{
		MeshComponent.Value->ClearInstances();
	}
	Grid.Empty();
}

void AProceduralGridActor::GenerateMesh()
{
	try {
		UE_LOG(LogTemp, Warning, TEXT("-----------------------GENERATING MESH-----------------------"));
		CleanUpMesh();

		//Load blocks
		if (Blocks.IsEmpty()) {
			UE_LOG(LogTemp, Warning, TEXT("No blocks found"));
			LoadBlocks();
		}

		//Check modules
		if (Modules.IsEmpty()) throw std::runtime_error("No modules found");


		for (auto ModuleClass : Modules) {
			if (ModuleClass == nullptr) throw std::runtime_error("Empty module detected");
		}
		
		//Process modules
		for (auto ModuleClass : Modules)
		{
			int CountBefore = Grid.Num();
			AModuleBase& Module = *ModuleClass->GetDefaultObject<AModuleBase>();
			FString ModuleName = Utility::CleanName(Module.GetName());
			UE_LOG(LogTemp, Warning, TEXT("Processing %s"), *ModuleName);
			int maxAttempts = 100;
			int attempt = 0;
			while (attempt < maxAttempts)
			{
				try {
					Module.Process();
					break;
				}
				catch (const std::exception&) {
					//UE_LOG(LogTemp, Error, TEXT("Exception: %s"), UTF8_TO_TCHAR(e.what()));
					attempt++;
				}
			}

			if (attempt >= maxAttempts) throw std::runtime_error("Failed to process module");

			UE_LOG(LogTemp, Warning, TEXT("%s finished (blocks: %d) (total blocks: %d) (Attempt: %d)"), *ModuleName, Grid.Num() - CountBefore, Grid.Num(), attempt);
		}

		SpawnMesh();
	}
	catch (const std::exception& e) {
		UE_LOG(LogTemp, Error, TEXT("Exception: %s"), UTF8_TO_TCHAR(e.what()));
	}
}


void AProceduralGridActor::SpawnMesh()
{
	if (Grid.IsEmpty()) return;

	auto pair = *Grid.begin();
	SpawnBlock(pair.Key, pair.Value);
	Grid.Remove(pair.Key);

	if (Delay <= 0 || Blocks[pair.Value].MeshId == 0) {
		SpawnMesh();
	}
	else {
		GetWorldTimerManager().SetTimer(DelayHandle, this, &AProceduralGridActor::SpawnMesh, Delay, false);
	}

}


void AProceduralGridActor::SpawnBlock(const FIntVector& position, const int& blockIndex)
{
	Block& block = GetBlock(blockIndex);
	if (block.MeshId == 0) return;

	FVector Location = FVector(position) * Offset - FVector(Size.X * Offset / 2, Size.Y * Offset / 2, -Offset / 2);

	FTransform Transform = FTransform(Location);
	Transform.SetRotation(FQuat(FRotator(0, block.Rotation, 0)));

	if (!InstancedMeshComponents.Contains(block.MeshId))
	{
		UE_LOG(LogTemp, Error, TEXT("No instanced mesh component for %d"), block.MeshId);
		return;
	}

	InstancedMeshComponents[block.MeshId]->AddInstance(Transform); //spawn mesh
}

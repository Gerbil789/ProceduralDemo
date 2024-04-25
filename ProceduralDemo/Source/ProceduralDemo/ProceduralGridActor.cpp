#include "ProceduralGridActor.h"
#include "Utility.h"

AProceduralGridActor::AProceduralGridActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void AProceduralGridActor::LoadBlocks()
{
	UE_LOG(LogTemp, Warning, TEXT("Loading blocks..."));
	CleanUpMesh();
	AllBlocks.Empty();

	if (BlueprintBlocks.Num() == 0)
	{
		throw std::runtime_error("No blocks to load");
	}

	for (auto block : BlueprintBlocks) {
		if (block.Block == nullptr)
		{
			throw std::runtime_error("Empty block detected");
		}
	}

	int meshId = 1;
	for (auto block : BlueprintBlocks)
	{
		AWFC_Base& base = *block.Block->GetDefaultObject<AWFC_Base>();

		if (base.Mesh == nullptr) {
			AllBlocks.Add(WFCBlock(0, 0, block.Priority, base)); //empty block
			continue;
		}

		for (int rotation = 0; rotation < base.Variants; rotation++)
		{
			int angle = rotation * -90;
			AllBlocks.Add(WFCBlock(meshId, angle, block.Priority, base));
		}

		//create instanced mesh component
		UInstancedStaticMeshComponent* NewInstancedMeshComponent = NewObject<UInstancedStaticMeshComponent>(this);
		NewInstancedMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		NewInstancedMeshComponent->SetStaticMesh(base.Mesh);
		NewInstancedMeshComponent->bUseDefaultCollision = true;
		NewInstancedMeshComponent->SetVisibility(true, false);
		InstancedMeshComponents.Add(meshId, NewInstancedMeshComponent);

		meshId++;
	}
	UE_LOG(LogTemp, Warning, TEXT("Loaded blocks count: %d"), AllBlocks.Num());
}

void AProceduralGridActor::CleanUpMesh()
{
	UE_LOG(LogTemp, Warning, TEXT("Cleaning up"));

	for (auto MeshComponent : InstancedMeshComponents)
	{
		MeshComponent.Value->ClearInstances();
	}
	DeterminedBlocks.Empty();
}

void AProceduralGridActor::GenerateMesh()
{
	try {
		UE_LOG(LogTemp, Warning, TEXT("-----------------------GENERATING MESH-----------------------"));
		CleanUpMesh();

		//Load blocks
		if (AllBlocks.IsEmpty()) {
			UE_LOG(LogTemp, Warning, TEXT("No blocks found"));
			LoadBlocks();
		}

		//Check modules
		if (ModuleClasses.IsEmpty()) throw std::runtime_error("No modules found");


		for (auto ModuleClass : ModuleClasses) {
			if (ModuleClass == nullptr) throw std::runtime_error("Empty module detected");
		}

		//Process modules
		for (auto ModuleClass : ModuleClasses)
		{
			AModuleBase& Module = *ModuleClass->GetDefaultObject<AModuleBase>();
			FString ModuleName = Utility::CleanName(Module.GetName());
			UE_LOG(LogTemp, Warning, TEXT("Processing %s"), *ModuleName);
			int maxAttempts = 5000;
			int attempt = 0;
			while(attempt < maxAttempts)
			{
				try {
					Module.Process(DeterminedBlocks, AllBlocks, Size);
					break;
				}
				catch (const std::exception&) {
					attempt++;
				}
			}

			if(attempt >= maxAttempts) throw std::runtime_error("Failed to process module (5000 attempts failed)");

			UE_LOG(LogTemp, Warning, TEXT("%s finished (total blocks: %d) (Attempt: %d)"), *ModuleName, DeterminedBlocks.Num(), attempt);
		}


		//Spawn mesh
		UE_LOG(LogTemp, Warning, TEXT("Spawning mesh (%d blocks)"), DeterminedBlocks.Num());
		for (auto& pair : DeterminedBlocks)
		{
			SpawnMesh(pair.Key, pair.Value);
		}
	}
	catch (const std::exception& e) {
		UE_LOG(LogTemp, Error, TEXT("Exception: %s"), UTF8_TO_TCHAR(e.what()));
	}
}


void AProceduralGridActor::SpawnMesh(const FIntVector& position, const WFCBlock& block)
{
	if (block.MeshId == 0) {
		//UE_LOG(LogTemp, Warning, TEXT("Spawned empty block"));
		return;
	}

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

#include "ProceduralGridActor.h"
#include "RoadGenerator.h"

AProceduralGridActor::AProceduralGridActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void AProceduralGridActor::LoadBlocks()
{
	UE_LOG(LogTemp, Warning, TEXT("Loading blocks"));
	Blocks.Empty();
	CleanUpMesh();

	int meshId = 1;
	for (auto block : BlueprintBlocks)
	{
		AWFC_Base& base = *block.Block->GetDefaultObject<AWFC_Base>();

		if (base.Mesh == nullptr) {
			//empty/fill block
			auto NewBlock = WFCBlock(0, 0, block.Priority, base);
			Blocks.Add(NewBlock);
			continue;
		}

		if (base.Variants >= 1) {
			auto NewBlock = WFCBlock(meshId, 0, block.Priority, base);
			Blocks.Add(NewBlock);
		}

		if (base.Variants >= 2) {
			auto NewBlock = WFCBlock(meshId, -90, block.Priority, base);
			Blocks.Add(NewBlock);
		}

		if (base.Variants >= 3) {
			auto NewBlock = WFCBlock(meshId, -180, block.Priority, base);
			Blocks.Add(NewBlock);
		}

		if (base.Variants >= 4) {
			auto NewBlock = WFCBlock(meshId, -270, block.Priority, base);
			Blocks.Add(NewBlock);
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
	UE_LOG(LogTemp, Warning, TEXT("Loaded blocks count: %d"), Blocks.Num());
}

void AProceduralGridActor::CleanUpMesh()
{
	UE_LOG(LogTemp, Warning, TEXT("Cleaning up"));

	for (auto MeshComponent : InstancedMeshComponents)
	{
		MeshComponent.Value->ClearInstances();
	}
}

void AProceduralGridActor::GenerateMesh()
{
	CleanUpMesh();

	if (Blocks.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("No blocks found"));
		return;
	}

	TArray<WFCBlock> RoadBlocks; //road blocks subset
	for (auto block : Blocks)
	{
		if (block.Type == BlockType::ROAD)
		{
			RoadBlocks.Add(block);
		}
	}

	TArray<WFCBlock> BuildingBlocks; //building blocks subset
	for (auto block : Blocks)
	{
		if (block.Type != BlockType::ROAD)
		{
			BuildingBlocks.Add(block);
		}
	}

	int attempt = 0;
	bool success = false;

	UE_LOG(LogTemp, Warning, TEXT("Generating ..."));
	while (attempt < 10 && !success) {
		try {
			attempt++;
			UE_LOG(LogTemp, Warning, TEXT("Attempt %d"), attempt);

			TMap<FIntVector, WFCBlock> Roads = RoadGenerator::GenerateRoads(RoadBlocks, Size);
			TMap<FIntVector, WFCBlock> BlocksToSpawn = AWaveFunctionCollapse::Generate(BuildingBlocks, Roads, Size);

			UE_LOG(LogTemp, Warning, TEXT("Spawning mesh"));
			for (auto& pair : BlocksToSpawn)
			{
				SpawnMesh(pair.Key, pair.Value);
			}
			success = true;
		}
		catch (const std::exception& e) {
			UE_LOG(LogTemp, Error, TEXT("Exception: %s"), UTF8_TO_TCHAR(e.what()));
		}
	}

	

		


}





void AProceduralGridActor::SpawnMesh(const FIntVector& position, const WFCBlock& block)
{
	//spawn mesh
	if (block.MeshId == 0) {
		UE_LOG(LogTemp, Warning, TEXT("Spawned empty block"));
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

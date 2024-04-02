#include "WaveFunctionCollapse.h"

AWaveFunctionCollapse::AWaveFunctionCollapse()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void AWaveFunctionCollapse::LoadBlocks()
{
	UE_LOG(LogTemp, Warning, TEXT("Loading blocks"));
	//clear blocks
	Blocks.Empty();

	CleanUpMesh();


	int meshId = 1;
	for (auto block : BlueprintBlocks)
	{
		AWFC_Base& base = *block.Block->GetDefaultObject<AWFC_Base>();

		if (base.Mesh == nullptr) {
			//empty/fill block
			auto NewBlock = WFCBlock(0, 0, base.Top, base.Bottom, base.Left, base.Right, base.Front, base.Back, block.Priority);
			Blocks.Add(NewBlock);
			continue;
		}

		if (base.Variants >= 1) {
			auto NewBlock = WFCBlock(meshId, 0, base.Top, base.Bottom, base.Left, base.Right, base.Front, base.Back, block.Priority);
			Blocks.Add(NewBlock);
		}

		if (base.Variants >= 2) {
			auto NewBlock = WFCBlock(meshId, -90, base.Top, base.Bottom, base.Back, base.Front, base.Left, base.Right, block.Priority);
			Blocks.Add(NewBlock);
		}

		if (base.Variants >= 3) {
			auto NewBlock = WFCBlock(meshId, -180, base.Top, base.Bottom, base.Right, base.Left, base.Back, base.Front, block.Priority);
			Blocks.Add(NewBlock);
		}

		if (base.Variants >= 4) {
			auto NewBlock = WFCBlock(meshId,-270, base.Top, base.Bottom, base.Front, base.Back, base.Right, base.Left, block.Priority);
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

void AWaveFunctionCollapse::CleanUpMesh()
{
	UE_LOG(LogTemp, Warning, TEXT("Cleaning up"));

	IndexGrid.Empty();
	CollapsedBlocks.Empty();

	for (auto MeshComponent : InstancedMeshComponents)
	{
		MeshComponent.Value->ClearInstances();
	}
}

void AWaveFunctionCollapse::GenerateMesh()
{
	CleanUpMesh();

	if (Blocks.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("No blocks found"));
		LoadBlocks();
		return;
	}

	infiniteLoopBreaker = 5000;
	UE_LOG(LogTemp, Warning, TEXT("Generating mesh"));

	//intialize grid
	for (int X = 0; X < Size.X; X++)
	{
		for (int Y = 0; Y < Size.Y; Y++)
		{
			for (int Z = 0; Z < Size.Z; Z++)
			{
				TArray<int> IndexArray;

				for (int i = 0; i < Blocks.Num(); ++i) {
					IndexArray.Add(i);
				}
				IndexGrid.Emplace(FIntVector(X, Y, Z), IndexArray);
			}
		}
	}

	////add empty tiles around
	//for (int X = -1; X < Size.X + 1; X++)
	//{
	//	for (int Y = -1; Y < Size.Y + 1; Y++)
	//	{
	//		
	//		if (!(X == -1 || X == Size.X || Y == -1 || Y == Size.Y)) continue;
	//
	//		CollapsedBlocks.Add(FIntVector(X, Y, 0), 2); //floor block	
	//	}
	//}



	////add fill tiles below
	//for (int X = 0; X < Size.X; X++)
	//{
	//	for (int Y = 0; Y < Size.Y; Y++)
	//	{
	//		CollapsedBlocks.Add(FIntVector(X, Y, -1), 1); //fill block
	//		Propagate(FIntVector(X, Y, 0));
	//	}
	//}


	//for (int X = 0; X < Size.X; X++)
	//{
	//	for (int Y = 0; Y < Size.Y; Y++)
	//	{
	//		CollapsedBlocks.Add(FIntVector(X, Y, Size.Z), 0); //empty block
	//	}
	//}

	WFC();
}

void AWaveFunctionCollapse::WFC()
{
	//check if grid is empty
	if (IndexGrid.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("WFCGrid is empty (Finished)"));
		return;
	}	

	//check for infinite loop
	if (infiniteLoopBreaker-- <= 0) {
		UE_LOG(LogTemp, Error, TEXT("Infinite loop detected"));
		return;
	}



	
	int LowestEntropy = 999;

	//find block (position) with lowest entropy
	TSet<FIntVector> LowestEntropySet = TSet<FIntVector>();

	for (auto IndexArray : IndexGrid)
	{
		int BlockEntropy = IndexArray.Value.Num();

		if (BlockEntropy == 0) {
			UE_LOG(LogTemp, Error, TEXT("BlockEntropy is 0 at position: %s"), *IndexArray.Key.ToString());
			IndexGrid.Remove(IndexArray.Key);
			continue;
		}

		if (BlockEntropy < LowestEntropy)
		{
			LowestEntropy = BlockEntropy;
			LowestEntropySet.Empty();
			LowestEntropySet.Add(IndexArray.Key);
		}
		else if (BlockEntropy == LowestEntropy)
		{
			LowestEntropySet.Add(IndexArray.Key);
		}
	}

	//pick random block from lowest entropy set
	FIntVector LowestEntropyBlockPosition = LowestEntropySet.Array()[FMath::RandRange(0, LowestEntropySet.Num() - 1)];


	//check if position is within grid
	if(!IndexGrid.Contains(LowestEntropyBlockPosition)) {
		UE_LOG(LogTemp, Error, TEXT("IndexGrid does not contain position: %s"), *LowestEntropyBlockPosition.ToString());
		return;
	}

	CollapseBlock(LowestEntropyBlockPosition);

	
	if (Delay == 0) {
		//call WFC again
		WFC();
	}
	else {
		//call WFC again with delay
		GetWorldTimerManager().SetTimer(DelayHandle, this, &AWaveFunctionCollapse::WFC, Delay, false);
	}

	
}




void AWaveFunctionCollapse::CollapseBlock(const FIntVector& position)
{
	//pick random block from lowest entropy set and remove the rest
	int totalPrioritySum = 0;
	for (auto i : IndexGrid[position]) {
		totalPrioritySum += Blocks[i].Priority;
	}
	int randomNumber = FMath::RandRange(0, totalPrioritySum);

	int blockIndex;
	for (auto i : IndexGrid[position]) {
		randomNumber -= Blocks[i].Priority;
		if (randomNumber <= 0) {
			blockIndex = i;
			break;
		}
	}

	WFCBlock& Block = Blocks[blockIndex];
	IndexGrid[position].Empty();
	IndexGrid[position].Add(blockIndex);


	//spawn mesh
	if (Block.MeshId == 0) {
		UE_LOG(LogTemp, Warning, TEXT("Spawned empty block"));
	}
	else {
		FVector Location = FVector(position) * Offset - FVector(Size.X * Offset / 2, Size.Y * Offset / 2, -Offset / 2);

		//UE_LOG(LogTemp, Warning, TEXT("SPAWNED STATICMESH_ID: %d | ROT: %d| POS[%d, %d, %d] | RIGHT: %s BACK: %s LEFT: %s FRONT: %s"), Block.MeshId, Block.Rotation, position.X, position.Y, position.Z, *Block.Right.ToString(), *Block.Back.ToString(), *Block.Left.ToString(), *Block.Front.ToString());

		FTransform Transform = FTransform(Location);
		Transform.SetRotation(FQuat(FRotator(0, Block.Rotation, 0)));

		if (!InstancedMeshComponents.Contains(Block.MeshId))
		{
			UE_LOG(LogTemp, Error, TEXT("No instanced mesh component for %d"), Block.MeshId);
			return;
		}

		InstancedMeshComponents[Block.MeshId]->AddInstance(Transform); //spawn mesh
	}

	//remove collapsed tile from grid
	IndexGrid.Remove(position);
	CollapsedBlocks.Add(position, blockIndex);

	// propagate constraints to neighbors
	Propagate(position + FIntVector(1, 0, 0));
	Propagate(position + FIntVector(-1, 0, 0));
	Propagate(position + FIntVector(0, 1, 0));
	Propagate(position + FIntVector(0, -1, 0));
	Propagate(position + FIntVector(0, 0, 1));
	Propagate(position + FIntVector(0, 0, -1));
}

void AWaveFunctionCollapse::Propagate(const FIntVector& position)
{
	//UE_LOG(LogTemp, Warning, TEXT("PROPAGATING CONSTRAINTS at [%d, %d, %d]"), position.X, position.Y, position.Z);

	if(!IndexGrid.Contains(position))
	{
		//UE_LOG(LogTemp, Error, TEXT("[PROPAGATE] IndexGrid does not contain position: %s"), *position.ToString());
		return;
	}

	RemoveInvalidBlocks(position, position + FIntVector(1, 0, 0));
	RemoveInvalidBlocks(position, position + FIntVector(-1, 0, 0));
	RemoveInvalidBlocks(position, position + FIntVector(0, 1, 0));
	RemoveInvalidBlocks(position, position + FIntVector(0, -1, 0));
	RemoveInvalidBlocks(position, position + FIntVector(0, 0, 1));
	RemoveInvalidBlocks(position, position + FIntVector(0, 0, -1));
}

void AWaveFunctionCollapse::RemoveInvalidBlocks(const FIntVector& positionToRemove, const FIntVector& positionToCompare)
{

	if (!IndexGrid.Contains(positionToRemove))
	{
		UE_LOG(LogTemp, Error, TEXT("[RemoveInvalidBlocks] IndexGrid does not contain position: %s"), *positionToRemove.ToString());
		return;
	}

	if (!CollapsedBlocks.Contains(positionToCompare))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Tile not collapsed: %s"), *positionToCompare.ToString());
		return;
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("Propagating constraints from %s to %s"), *positionToRemove.ToString(), *positionToCompare.ToString());

	//UE_LOG(LogTemp, Warning, TEXT("Entropy before propagation: %d"), IndexGrid[positionToRemove].Num());
	TSet<int> BlocksToRemove = TSet<int>();

	FIntVector direction = positionToCompare - positionToRemove;

	if(direction == FIntVector(1, 0, 0)) 
	{
		for (auto Index : IndexGrid[positionToRemove])
		{
			if (Blocks[Index].Right == Blocks[CollapsedBlocks[positionToCompare]].Left) {
				continue;
			}
			BlocksToRemove.Add(Index);
		}
		
	}
	else if (direction == FIntVector(-1, 0, 0))
	{
		for (auto Index : IndexGrid[positionToRemove])
		{
			if (Blocks[Index].Left == Blocks[CollapsedBlocks[positionToCompare]].Right) {
				continue;
			}
			BlocksToRemove.Add(Index);
		}
	}
	else if (direction == FIntVector(0, 1, 0))
	{
		for (auto Index : IndexGrid[positionToRemove])
		{
			if (Blocks[Index].Front == Blocks[CollapsedBlocks[positionToCompare]].Back) {
				continue;
			}
			BlocksToRemove.Add(Index);
		}
	}
	else if (direction == FIntVector(0, -1, 0))
	{
		for (auto Index : IndexGrid[positionToRemove])
		{
			if (Blocks[Index].Back == Blocks[CollapsedBlocks[positionToCompare]].Front) {
				continue;
			}
			BlocksToRemove.Add(Index);
		}
	}
	else if (direction == FIntVector(0, 0, 1))
	{
		for (auto Index : IndexGrid[positionToRemove])
		{
			if (Blocks[Index].Top == Blocks[CollapsedBlocks[positionToCompare]].Bottom) {
				continue;
			}
			BlocksToRemove.Add(Index);
		}
	}
	else if (direction == FIntVector(0, 0, -1))
	{
		for (auto Index : IndexGrid[positionToRemove])
		{
			if (Blocks[Index].Bottom == Blocks[CollapsedBlocks[positionToCompare]].Top) {
				continue;
			}
			BlocksToRemove.Add(Index);
		}
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid direction"));
		return;
	}

	for(auto Index : BlocksToRemove)
	{
		IndexGrid[positionToRemove].Remove(Index);
	}

	/*UE_LOG(LogTemp, Warning, TEXT("Entropy after propagation: %d"), IndexGrid[positionToRemove].Num());

	UE_LOG(LogTemp, Warning, TEXT("Remaining blocks:"));

	for(auto Index : IndexGrid[positionToRemove])
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Blocks[Index].ToString());
	}*/
}







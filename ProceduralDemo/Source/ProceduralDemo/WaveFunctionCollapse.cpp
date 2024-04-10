#include "WaveFunctionCollapse.h"

const TArray<FIntVector> AWaveFunctionCollapse::Directions = { FIntVector(1, 0, 0),FIntVector(-1, 0, 0), FIntVector(0, 1, 0), FIntVector(0, -1, 0), FIntVector(0, 0, 1), FIntVector(0, 0, -1) };

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
			auto NewBlock = WFCBlock(meshId, -270, base.Top, base.Bottom, base.Front, base.Back, base.Right, base.Left, block.Priority);
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
		return;
	}

	infiniteLoopBreaker = 10000;
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

	while (IndexGrid.IsEmpty() == false && IndexGrid.Num() != 0) {

		if (infiniteLoopBreaker-- <= 0) {
			UE_LOG(LogTemp, Error, TEXT("Infinite loop detected"));
			return;
		}

		FIntVector lowestEntropyPosition = GetLowestEntropyPosition();

		Collapse(lowestEntropyPosition);
		Propagate(lowestEntropyPosition);
	}

	SpawnBlocks();
}

FIntVector AWaveFunctionCollapse::GetLowestEntropyPosition()
{
	int LowestEntropy = TNumericLimits<int32>::Max();

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

	if (!LowestEntropySet.IsEmpty()) {
		FIntVector LowestEntropyBlockPosition = LowestEntropySet.Array()[FMath::RandRange(0, LowestEntropySet.Num() - 1)];
		return LowestEntropyBlockPosition;
	}
	else {
		return FIntVector(-1, -1, -1);
	}
	}
	
void AWaveFunctionCollapse::Collapse(const FIntVector& position)
{
	//pick random block from lowest entropy set and remove the rest
	int blockIndex;
	if (IndexGrid[position].Num() > 1)
	{
		int totalPrioritySum = 0;
		for (auto i : IndexGrid[position]) {
			totalPrioritySum += Blocks[i].Priority;
		}
		int randomNumber = FMath::RandRange(0, totalPrioritySum);

		for (auto i : IndexGrid[position]) {
			randomNumber -= Blocks[i].Priority;
			if (randomNumber <= 0) {
				blockIndex = i;
				break;
			}
		}
	}
	else
	{
		blockIndex = IndexGrid[position][0];
	}

	IndexGrid.Remove(position);
	CollapsedBlocks.Add(position, blockIndex);
	CalculatedBlocks.push(std::make_pair(position, blockIndex)); //spawn mesh later
}

void AWaveFunctionCollapse::Propagate(const FIntVector& position)
{
	std::stack<FIntVector> Stack;
	TSet<FIntVector> InStack;

	int tmp = 0;
	Stack.push(position);
	InStack.Add(position);

	while (!Stack.empty()) {
		tmp++;
		if (tmp > 10000) {
			UE_LOG(LogTemp, Error, TEXT("Infinite loop detected inside propagation"));
			return;
		}

		FIntVector currentPosition = Stack.top();
		Stack.pop();


		for (auto Direction : Directions)
		{
			FIntVector newPosition = currentPosition + Direction;
			if (!IndexGrid.Contains(newPosition) || InStack.Contains(newPosition)) continue;

			TSet<int> blocksToRemove = GetBlocksToRemove(position, Direction);
			if (blocksToRemove.IsEmpty() || blocksToRemove.Num() == 0) continue;

			if (blocksToRemove.Num() == IndexGrid[newPosition].Num()) {
				UE_LOG(LogTemp, Error, TEXT("No valid blocks to propagate at position: %s"), *newPosition.ToString());
				continue;
			}


			for (auto blockIndex : blocksToRemove)
			{
				IndexGrid[newPosition].Remove(blockIndex);
			}

			if (IndexGrid[newPosition].Num() == 0) {
				UE_LOG(LogTemp, Error, TEXT("No blocks left at position: %s"), *newPosition.ToString());
				IndexGrid.Remove(newPosition);
				continue;
			}


			Stack.push(newPosition);
			InStack.Add(newPosition);
		}

		InStack.Remove(currentPosition);

	}
}

TSet<int> AWaveFunctionCollapse::GetBlocksToRemove(const FIntVector& position, const FIntVector& direction)
{
	TSet<int> BlocksToRemove;
	if (CollapsedBlocks.Contains(position + direction)) return BlocksToRemove;
	if(!IndexGrid.Contains(position + direction)) return BlocksToRemove;

	int CollapsedIndex = CollapsedBlocks[position];

	for (auto Index : IndexGrid[position + direction])
	{
		if ((direction.X == 1 && Blocks[CollapsedIndex].Right != Blocks[Index].Left) ||
			(direction.X == -1 && Blocks[CollapsedIndex].Left != Blocks[Index].Right) ||
			(direction.Y == 1 && Blocks[CollapsedIndex].Front != Blocks[Index].Back) ||
			(direction.Y == -1 && Blocks[CollapsedIndex].Back != Blocks[Index].Front) ||
			(direction.Z == 1 && Blocks[CollapsedIndex].Top != Blocks[Index].Bottom) ||
			(direction.Z == -1 && Blocks[CollapsedIndex].Bottom != Blocks[Index].Top))
		{
			BlocksToRemove.Add(Index);
		}
	}
	return BlocksToRemove;
}

void AWaveFunctionCollapse::SpawnBlocks()
{
	if (Delay == 0) {
		while (CalculatedBlocks.empty() == false)
		{
			auto block = CalculatedBlocks.top();
			CalculatedBlocks.pop();
			SpawnMesh(block.first, block.second);
		}

	}
	else {
		while (CalculatedBlocks.empty() == false)
		{
			auto block = CalculatedBlocks.top();
			CalculatedBlocks.pop();
			SpawnMesh(block.first, block.second);
		}
	}
}


void AWaveFunctionCollapse::SpawnMesh(const FIntVector& position, const int& blockIndex)
{
	WFCBlock& Block = Blocks[blockIndex];

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
}

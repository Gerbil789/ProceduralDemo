#include "WaveFunctionCollapse.h"

AWaveFunctionCollapse::AWaveFunctionCollapse()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void AWaveFunctionCollapse::LoadDataTable()
{
	//clear blocks
	Blocks.Empty();

	if (!MyDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("No datatable found"));
		return;
	}

	//add empty block (no mesh)
	WFCBlock emptyBlock = WFCBlock("empty", 0, "0i", "0i", "0s", "0s", "0s", "0s");
	Blocks.Add(emptyBlock);

	//add fill block (no mesh)
	WFCBlock fillBlock = WFCBlock("fill", 0, "1i", "1i", "1s", "1s", "1s", "1s");
	Blocks.Add(fillBlock);

	//load blocks from datatable
	TArray<FName> RowNames = MyDataTable->GetRowNames();
	for (auto RowName : RowNames)
	{
		//find static mesh
		FString AssetPath = FString::Printf(TEXT("/Game/Meshes/WFC_Demo/WFC_Blocks_%s"), *RowName.ToString());
		UStaticMesh* StaticMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *AssetPath));
		if (!StaticMesh)
		{
			UE_LOG(LogTemp, Error, TEXT("StaticMesh not found: %s"), *AssetPath);
			continue;
		}

		//find row in datatable
		FWFCBlockDataStruct* Row = MyDataTable->FindRow<FWFCBlockDataStruct>(RowName, TEXT("LookupBlockData"));
		if (!Row) {
			UE_LOG(LogTemp, Warning, TEXT("Failed to find row: %s"), *RowName.ToString());
			continue;
		}

		//create block 
		TArray<FString> HorizontalSockets = { Row->Left, Row->Back, Row->Right, Row->Front };

		FString meshName = FString::Printf(TEXT("WFC_Blocks_%s"), *RowName.ToString());
		WFCBlock NewBlock = WFCBlock(meshName, 0, Row->Top, Row->Bottom, HorizontalSockets[0], HorizontalSockets[2], HorizontalSockets[3], HorizontalSockets[1]);
		Blocks.Add(NewBlock);

		//create instanced mesh component
		UInstancedStaticMeshComponent* NewInstancedMeshComponent = NewObject<UInstancedStaticMeshComponent>(this);
		NewInstancedMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		NewInstancedMeshComponent->SetStaticMesh(StaticMesh);
		InstancedMeshComponents.Add(meshName, NewInstancedMeshComponent);

		//add rotated blocks
		for (int i = 1; i < Row->Variants; i++) {
			std::rotate(HorizontalSockets.GetData(), HorizontalSockets.GetData() + 1, HorizontalSockets.GetData() + HorizontalSockets.Num());

			NewBlock = WFCBlock(meshName, i * (-90), Row->Top, Row->Bottom, HorizontalSockets[0], HorizontalSockets[2], HorizontalSockets[3], HorizontalSockets[1]);
			Blocks.Add(NewBlock);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Loaded blocks count: %d"), Blocks.Num());
	//for(auto Block : Blocks) {
	//	UE_LOG(LogTemp, Warning, TEXT("%s"), *Block.ToString());
	//}
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

	//add empty tiles around
	for (int X = -1; X < Size.X + 1; X++)
	{
		for (int Y = -1; Y < Size.Y + 1; Y++)
		{
			
			if (!(X == -1 || X == Size.X || Y == -1 || Y == Size.Y)) continue;
	
			CollapsedBlocks.Add(FIntVector(X, Y, 0), 2); //floor block	
		}
	}



	//add fill tiles below
	for (int X = 0; X < Size.X; X++)
	{
		for (int Y = 0; Y < Size.Y; Y++)
		{
			CollapsedBlocks.Add(FIntVector(X, Y, -1), 1); //fill block
			Propagate(FIntVector(X, Y, 0));
		}
	}


	for (int X = 0; X < Size.X; X++)
	{
		for (int Y = 0; Y < Size.Y; Y++)
		{
			CollapsedBlocks.Add(FIntVector(X, Y, Size.Z), 0); //empty block
		}
	}


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

	FIntVector LowestEntropyBlockPosition = FIntVector(0,0,0);
	int LowestEntropy = 999;

	//find block (position) with lowest entropy
	for (auto IndexArray : IndexGrid)
	{
		int BlockEntropy = IndexArray.Value.Num();

		if (BlockEntropy == 0) {
			UE_LOG(LogTemp, Error, TEXT("BlockEntropy is 0 at position: %s"), *IndexArray.Key.ToString());
			IndexGrid.Remove(IndexArray.Key);
			continue;
		}

		//todo: add randomization (set -> pick random)
		if (BlockEntropy < LowestEntropy)
		{
			LowestEntropy = BlockEntropy;
			LowestEntropyBlockPosition = IndexArray.Key;
		}
	}

	//check if position is within grid
	if(!IndexGrid.Contains(LowestEntropyBlockPosition)) {
		UE_LOG(LogTemp, Error, TEXT("IndexGrid does not contain position: %s"), *LowestEntropyBlockPosition.ToString());
		return;
	}

	CollapseBlock(LowestEntropyBlockPosition);

	//call WFC again with delay
	GetWorldTimerManager().SetTimer(DelayHandle, this, &AWaveFunctionCollapse::WFC, Delay, false);
}




void AWaveFunctionCollapse::CollapseBlock(const FIntVector& position)
{
	//pick random block from lowest entropy set and remove the rest
	int BlockIndex = IndexGrid[position][FMath::RandRange(0, IndexGrid[position].Num() - 1)];
	WFCBlock& Block = Blocks[BlockIndex];
	IndexGrid[position].Empty();
	IndexGrid[position].Add(BlockIndex);


	//spawn mesh
	if (Block.Mesh == "empty" || Block.Mesh == "fill" || Block.Mesh == "empty_floor") {
		UE_LOG(LogTemp, Warning, TEXT("Spawned empty/fill block"));
	}
	else {
		FVector Location = FVector(position) * Offset - FVector(Size.X * Offset / 2, Size.Y * Offset / 2, -Offset / 2);

		UE_LOG(LogTemp, Warning, TEXT("SPAWNED STATICMESH: %s | ROT: %d| POS[%d, %d, %d] | RIGHT: %s BACK: %s LEFT: %s FRONT: %s"), *Block.Mesh, Block.Rotation, position.X, position.Y, position.Z, *Block.Right.ToString(), *Block.Back.ToString(), *Block.Left.ToString(), *Block.Front.ToString());

		FTransform Transform = FTransform(Location);
		Transform.SetRotation(FQuat(FRotator(0, Block.Rotation, 0)));

		if (!InstancedMeshComponents.Contains(Block.Mesh))
		{
			UE_LOG(LogTemp, Error, TEXT("No instanced mesh component for %s"), *Block.Mesh);
			return;
		}

		InstancedMeshComponents[Block.Mesh]->AddInstance(Transform);
	}

	//remove collapsed tile from grid
	IndexGrid.Remove(position);
	CollapsedBlocks.Add(position, BlockIndex);

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

	//UE_LOG(LogTemp, Warning, TEXT("Entropy after propagation: %d"), IndexGrid[positionToRemove].Num());

	//UE_LOG(LogTemp, Warning, TEXT("Remaining blocks:"));

	//for(auto Index : IndexGrid[positionToRemove])
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("%s"), *Blocks[Index].ToString());
	//}
}







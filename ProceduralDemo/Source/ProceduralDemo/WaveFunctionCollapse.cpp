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
	WFCBlock emptyBlock = WFCBlock("", 0, "0i", "0i", "0s", "0s", "0s", "0s");
	Blocks.Add(emptyBlock);

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
		WFCBlock NewBlock = WFCBlock(meshName, 0, Row->Up, Row->Down, HorizontalSockets[0], HorizontalSockets[2], HorizontalSockets[3], HorizontalSockets[1]);
		Blocks.Add(NewBlock);

		//create instanced mesh component
		UInstancedStaticMeshComponent* NewInstancedMeshComponent = NewObject<UInstancedStaticMeshComponent>(this);
		NewInstancedMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		NewInstancedMeshComponent->SetStaticMesh(StaticMesh);
		InstancedMeshComponents.Add(meshName, NewInstancedMeshComponent);

		//add rotated blocks
		for (int i = 1; i < Row->Variants; i++) {
			std::rotate(HorizontalSockets.GetData(), HorizontalSockets.GetData() + 1, HorizontalSockets.GetData() + HorizontalSockets.Num());

			NewBlock = WFCBlock(meshName, i * 90, Row->Up, Row->Down, HorizontalSockets[0], HorizontalSockets[2], HorizontalSockets[3], HorizontalSockets[1]);
			Blocks.Add(NewBlock);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Loaded blocks count: %d"), Blocks.Num());
}



void AWaveFunctionCollapse::CleanUpMesh()
{
	UE_LOG(LogTemp, Warning, TEXT("Cleaning up"));

	IndexGrid.Empty();

	for (auto MeshComponent : InstancedMeshComponents)
	{
		MeshComponent.Value->ClearInstances();
	}
}

void AWaveFunctionCollapse::GenerateMesh()
{
	CleanUpMesh();
	infiniteLoopBreaker = 1000;
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

	WFC();
}


void AWaveFunctionCollapse::WFC()
{
	//check if grid is empty
	if (IndexGrid.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("WFCGrid is empty (Finished?)"));
		return;
	}	

	//check for infinite loop
	if (infiniteLoopBreaker-- <= 0) {
		UE_LOG(LogTemp, Error, TEXT("Infinite loop detected"));
		return;
	}

	FIntVector LowestEntropyBlockPosition;
	int LowestEntropy = 999;

	//find block (position) with lowest entropy
	for (auto IndexArray : IndexGrid)
	{
		int BlockEntropy = IndexArray.Value.Num();

		if (BlockEntropy == 0) {
			IndexGrid.Remove(IndexArray.Key);
			continue;
		}

		if (BlockEntropy < LowestEntropy)
		{
			LowestEntropy = BlockEntropy;
			LowestEntropyBlockPosition = IndexArray.Key;
		}
	}

	//pick random block from lowest entropy set and remove the rest
	int BlockIndex = IndexGrid[LowestEntropyBlockPosition][FMath::RandRange(0, LowestEntropy - 1)];
	WFCBlock& Block = Blocks[BlockIndex];
	IndexGrid[LowestEntropyBlockPosition].Empty();
	IndexGrid[LowestEntropyBlockPosition].Add(BlockIndex);

	SpawnMesh(Block, LowestEntropyBlockPosition);

	// propagate constraints to neighbors
	Propagate(Block, LowestEntropyBlockPosition);

	//remove collapsed tile from grid
	IndexGrid[LowestEntropyBlockPosition].Empty();
	IndexGrid.Remove(LowestEntropyBlockPosition);

	//call WFC again with delay
	GetWorldTimerManager().SetTimer(DelayHandle, this, &AWaveFunctionCollapse::WFC, Delay, false);
}

void AWaveFunctionCollapse::Propagate(const WFCBlock& block, const FIntVector& position)
{
	//TArray<FString> Directions = { FIntVector(1, 0, 0), FIntVector(-1, 0, 0), FIntVector(0, 1, 0), FIntVector(0, -1, 0), FIntVector(0, 0, 1), FIntVector(0, 0, -1) };
	TArray<FString> Directions = { "Up", "Down", "Left", "Right", "Front", "Back" };
	for (auto dir : Directions) {
		RemoveInvalidBlocks(block, position, dir);
	}
}

void AWaveFunctionCollapse::SpawnMesh(const WFCBlock& block, const FIntVector& position)
{
	if (block.Mesh == "") {
		UE_LOG(LogTemp, Warning, TEXT("Spawned empty block"));
		return;
	}

	FVector Location = FVector(position) * Offset - FVector(Size.X * Offset / 2, Size.Y * Offset / 2, -Offset / 2);

	UE_LOG(LogTemp, Warning, TEXT("Spawning tile at location: %s"), *Location.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Rotation: %d"), block.Rotation);
	UE_LOG(LogTemp, Warning, TEXT("StaticMesh: %s"), *block.Mesh);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *block.ToString());

	FTransform Transform = FTransform(Location);
	Transform.SetRotation(FQuat(FRotator(0, block.Rotation, 0)));
		
	if (!InstancedMeshComponents.Contains(block.Mesh))
	{
		UE_LOG(LogTemp, Error, TEXT("No instanced mesh component for %s"), *block.Mesh);
		return;
	}

	InstancedMeshComponents[block.Mesh]->AddInstance(Transform);
}

void AWaveFunctionCollapse::RemoveInvalidBlocks( const WFCBlock& block, const FIntVector& position, const FString& direction )
{
	FIntVector NeighborOffset;

	if(direction == "Up") {
		NeighborOffset = FIntVector(0, 0, 1);
	}
	else if (direction == "Down") {
		NeighborOffset = FIntVector(0, 0, -1);
	}
	else if (direction == "Left") {
		NeighborOffset = FIntVector(1, 0, 0);
	}
	else if (direction == "Right") {
		NeighborOffset = FIntVector(-1, 0, 0);
	}
	else if (direction == "Front") {
		NeighborOffset = FIntVector(0, 1, 0);
	}
	else if (direction == "Back") {
		NeighborOffset = FIntVector(0, -1, 0);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Invalid direction: %s"), *direction);
		return;
	}

	FIntVector neighborPosition = position + NeighborOffset;


	//check if position is within grid
	if (!IndexGrid.Contains(neighborPosition))
	{
		return;
	}

	TSet<int> ElementsToRemove = TSet<int>();

	UE_LOG(LogTemp, Warning, TEXT("IndexGrid[%d, %d, %d] count: %d"), neighborPosition.X, neighborPosition.Y, neighborPosition.Z, IndexGrid[neighborPosition].Num());

	for (auto Index : IndexGrid[neighborPosition])
	{
		if (!CompareSockets(block, Blocks[Index], direction)) {
			ElementsToRemove.Add(Index);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("ElementsToRemove count: %d"), ElementsToRemove.Num());

	for (const auto& Element : ElementsToRemove)
	{
		IndexGrid[neighborPosition].Remove(Element);
	}
}

bool AWaveFunctionCollapse::CompareSockets(const WFCBlock& block, const WFCBlock& neighborBlock, const FString& direction)
{
	if(direction == "Up") {
		return block.Top == neighborBlock.Bottom;
	}
	else if (direction == "Down") {
		return block.Bottom == neighborBlock.Top;
	}
	else if (direction == "Left") {
		return block.Left == neighborBlock.Right;
	}
	else if (direction == "Right") {
		return block.Right == neighborBlock.Left;
	}
	else if (direction == "Front") {
		return block.Front == neighborBlock.Back;
	}
	else if (direction == "Back") {
		return block.Back == neighborBlock.Front;
	}
	
	return false;
}







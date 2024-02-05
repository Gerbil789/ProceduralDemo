#include "ProceduralBuilding.h"


AProceduralBuilding::AProceduralBuilding()
{

	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	WallInstancedMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("WallInstancedMeshComponent"));
	WallInstancedMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	DoorInstancedMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("DoorInstancedMeshComponent"));
	DoorInstancedMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	WindowInstancedMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("WindowInstancedMeshComponent"));
	WindowInstancedMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

AProceduralBuilding::~AProceduralBuilding()
{


}

void AProceduralBuilding::GenerateMesh()
{
	CleanUpMesh();
	UE_LOG(LogTemp, Warning, TEXT("Generating mesh"));
	//generate random points
	for(int X = 0; X < Size.X; X++)
	{
		for(int Y = 0; Y < Size.Y; Y++)
		{
			for(int Z = 0; Z < Size.Z; Z++)
			{
				if (Z == 0 || BuildingBlocks.Contains(FIntVector(X, Y, Z - 1)))
				{
					if(FMath::RandRange(0.0f, 1.0f) <= Density)
					{
						BuildingBlocks.Add(FIntVector(X, Y, Z));
					}
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Blocks count: %d"), BuildingBlocks.Num());

	if(WallMesh.IsEmpty() || DoorMesh.IsEmpty() || WindowMesh.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Meshes not set"));
		return;
	}

	WallInstancedMeshComponent->SetStaticMesh(WallMesh[FMath::RandRange(0, WallMesh.Num() - 1)]);
	DoorInstancedMeshComponent->SetStaticMesh(DoorMesh[FMath::RandRange(0, DoorMesh.Num() - 1)]);
	WindowInstancedMeshComponent->SetStaticMesh(WindowMesh[FMath::RandRange(0, WindowMesh.Num() - 1)]);

	for(auto Block : BuildingBlocks)
	{
		SpawnBlock(Block);
	}

	UE_LOG(LogTemp, Warning, TEXT("Mesh generated"));
}

void AProceduralBuilding::CleanUpMesh()
{
	UE_LOG(LogTemp, Warning, TEXT("Cleaning up mesh"));

	BuildingBlocks.Empty();

	if(WallInstancedMeshComponent)
		WallInstancedMeshComponent->ClearInstances();

	if(DoorInstancedMeshComponent)
		DoorInstancedMeshComponent->ClearInstances();

	if(WindowInstancedMeshComponent)
		WindowInstancedMeshComponent->ClearInstances();

	UE_LOG(LogTemp, Warning, TEXT("Mesh cleaned up"));
}

void AProceduralBuilding::SpawnBlock(FIntVector Position)
{
	FVector Location = FVector(Position) * Offset - FVector(Size.X * Offset / 2, Size.Y * Offset / 2, -Offset / 2);

	UE_LOG(LogTemp, Warning, TEXT("Block location: %s"), *Location.ToString());

	FIntVector NeighborPosition = Position + FIntVector(-1, 0, 0);
	if(!BuildingBlocks.Contains(NeighborPosition))
	{
		FTransform Transform = FTransform(Location + FVector(-Offset / 2, 0, 0));
		Transform.SetRotation(FQuat(FRotator(0, 90, 0)));
		if (Position.Z == 0) {
			if (FMath::RandRange(0.0f, 1.0f) <= 0.2f) {
				DoorInstancedMeshComponent->AddInstance(Transform);
			}
			else {
				WallInstancedMeshComponent->AddInstance(Transform);
			}	
		}
		else {
			WindowInstancedMeshComponent->AddInstance(Transform);
		}
	}

	NeighborPosition = Position + FIntVector(1, 0, 0);
	if (!BuildingBlocks.Contains(NeighborPosition))
	{
		FTransform Transform = FTransform(Location + FVector(Offset / 2, 0, 0));
		Transform.SetRotation(FQuat(FRotator(0, -90, 0)));
		if (Position.Z == 0) {
			if (FMath::RandRange(0.0f, 1.0f) <= 0.2f) {
				DoorInstancedMeshComponent->AddInstance(Transform);
			}
			else {
				WallInstancedMeshComponent->AddInstance(Transform);
			}
		}
		else {
			WindowInstancedMeshComponent->AddInstance(Transform);
		}
	}

	NeighborPosition = Position + FIntVector(0, -1, 0);
	if (!BuildingBlocks.Contains(NeighborPosition))
	{
		FTransform Transform = FTransform(Location + FVector(0, -Offset / 2, 0));
		Transform.SetRotation(FQuat(FRotator(0, 180, 0)));
		if (Position.Z == 0) {
			if (FMath::RandRange(0.0f, 1.0f) <= 0.2f) {
				DoorInstancedMeshComponent->AddInstance(Transform);
			}
			else {
				WallInstancedMeshComponent->AddInstance(Transform);
			}
		}
		else {
			WindowInstancedMeshComponent->AddInstance(Transform);
		}
	}

	NeighborPosition = Position + FIntVector(0, 1, 0);
	if (!BuildingBlocks.Contains(NeighborPosition))
	{
		FTransform Transform = FTransform(Location + FVector(0, Offset / 2, 0));
		Transform.SetRotation(FQuat(FRotator(0, 0, 0)));
		if (Position.Z == 0) {
			if (FMath::RandRange(0.0f, 1.0f) <= 0.2f) {
				DoorInstancedMeshComponent->AddInstance(Transform);
			}
			else {
				WallInstancedMeshComponent->AddInstance(Transform);
			}
		}
		else {
			WindowInstancedMeshComponent->AddInstance(Transform);
		}
	}


	NeighborPosition = Position + FIntVector(0, 0, -1);
	if (!BuildingBlocks.Contains(NeighborPosition))
	{
		FTransform Transform = FTransform(Location + FVector(0, 0, -Offset / 2));
		Transform.SetRotation(FQuat(FRotator(0, 0, 90)));
		WallInstancedMeshComponent->AddInstance(Transform);
	}

	NeighborPosition = Position + FIntVector(0, 0, 1);
	if (!BuildingBlocks.Contains(NeighborPosition))
	{
		FTransform Transform = FTransform(Location + FVector(0, 0, Offset / 2));
		Transform.SetRotation(FQuat(FRotator(0, 0, -90)));
		WallInstancedMeshComponent->AddInstance(Transform);
	}

}


#include "MarchingCubesActor.h"


AMarchingCubesActor::AMarchingCubesActor()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set up a root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}


void AMarchingCubesActor::GenerateMesh(const FIntVector& size)
{
	UE_LOG(LogTemp, Warning, TEXT("Generate started"));
	CleanUpMesh();
	GeneratePoints(size, 0.0f, 1.0f);

	for (const auto& point : Points) {
		// Retrieve the point's location and radius
		FVector pointLocation = point.Key;
		float pointColorFactor = point.Value;

		SpawnDebugSphere(point.Key, point.Value);
		
	}

	UE_LOG(LogTemp, Warning, TEXT("Generate finished"));
}

void AMarchingCubesActor::CleanUpMesh()
{
	UE_LOG(LogTemp, Warning, TEXT("Cleanup started"));

	if(!Points.IsEmpty())
	{
		Points.Empty();
		Points.Compact(); // Compact() is not necessary, but it's good practice to call it after emptying an array
	}

	for (AActor* sphereActor : DebugSpheres)
	{
		if (sphereActor != nullptr)
		{
			sphereActor->Destroy();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Cleanup finished"));
}

void AMarchingCubesActor::GeneratePoints(const FIntVector& size, float min, float max)
{
	UE_LOG(LogTemp, Warning, TEXT("GeneratePoints started"));

	if (size.X <= 0 || size.Y <= 0 || size.Z <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid size"));
		return;
	}

	Points.Empty();
	for (int32 z = 0; z < size.Z; z++)
	{
		for (int32 y = 0; y < size.Y; y++)
		{
			for (int32 x = 0; x < size.X; x++)
			{
				Points.Add(FVector(x, y, z) * 100, FMath::FRandRange(min, max));
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("%d points generated"), Points.Num());
}

void AMarchingCubesActor::SpawnDebugSphere(const FVector& location, const float& value)
{
	if(DebugSphereActorBlueprint == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("DebugSphereActorBlueprint is not set in the editor for %s"), *GetName());
		return;
	}
	FActorSpawnParameters SpawnParams;
	ADebugSphereActor* DebugSphereInstance = GetWorld()->SpawnActor<ADebugSphereActor>(DebugSphereActorBlueprint, SpawnParams);

	if (DebugSphereInstance == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("DebugSphereInstance is null"));
		return;
	}

	DebugSphereInstance->SetActorLocation(location);
	DebugSphereInstance->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	DebugSphereInstance->SetValue(value);
	DebugSpheres.Add(DebugSphereInstance);
}



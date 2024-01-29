#include "MarchingCubesActor.h"
#include "ProceduralMeshComponent.h"

// optimization
// vertices are duplicating for each cube
// use mutlithreading
// use chunks for infinite world

AMarchingCubesActor::AMarchingCubesActor()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	//ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
	//ProceduralMeshComponent->SetupAttachment(GetRootComponent());


}


void AMarchingCubesActor::GenerateMesh(const FIntVector& size, const float& surfaceLevel)
{
	UE_LOG(LogTemp, Warning, TEXT("Generate started"));
	SurfaceLevel = surfaceLevel;
	CleanUpMesh();


	ProceduralMeshComponent = this->FindComponentByClass<UProceduralMeshComponent>();
	if (!ProceduralMeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ProceduralMeshComponent not found"));
		return;
	}
	ProceduralMeshComponent->ClearAllMeshSections();


	GeneratePoints(size, 0.0f, 1.0f);

	//Spawn Debug Spheres
	for (const auto& point : Points) {
		FVector pointLocation = point.Key;
		float pointColorFactor = point.Value;
		SpawnDebugSphere(point.Key, point.Value);
	}

	for (int32 X = 0; X < size.X - 1; ++X) {
		for (int32 Y = 0; Y < size.Y - 1; ++Y) {
			for (int32 Z = 0; Z < size.Z - 1; ++Z) {

				Cube cube;
				FVector CubeOrigin(X, Y, Z);
				UE_LOG(LogTemp, Error, TEXT("Cube %s"), *CubeOrigin.ToString());

				// Determine the scalar values at the cube corners

				for (int32 i = 0; i < 8; ++i) {
					FVector Corner = CubeOrigin + GetCornerOffset(i);
					if (!Points.Contains(Corner)) {
						UE_LOG(LogTemp, Error, TEXT("Corner %d not found in Points for %s"), i, *CubeOrigin.ToString());
						return;
					}
					cube.Values[i] = Points[Corner];
					cube.Points[i] = Corner;
				}

				// Determine the cube configuration (binary representation)
				cube.CubeIndex = 0;
				for (int32 i = 0; i < 8; ++i) {
					if (cube.Values[i] < SurfaceLevel)
						cube.CubeIndex |= 1 << i;
				}

				// Skip the cube if it is entirely inside or outside of the surface
				if (EdgeTable[cube.CubeIndex] == 0) {
					continue;
				}

				GenerateCubeMesh(cube);
			}
		}
	}


	UE_LOG(LogTemp, Warning, TEXT("Generate finished"));
}

FVector AMarchingCubesActor::GetCornerOffset(int32 CornerIndex) {
	// Helper function to get the offset for each cube corner
	// Adjust based on your specific cube configuration
	// For a 2x2x2 cube, you can use a simple lookup table
	static const FVector CornerOffsets[8] = {
		FVector(0, 0, 0), FVector(1, 0, 0), FVector(1, 1, 0), FVector(0, 1, 0),
		FVector(0, 0, 1), FVector(1, 0, 1), FVector(1, 1, 1), FVector(0, 1, 1)
	};
	return CornerOffsets[CornerIndex];
}

void AMarchingCubesActor::GenerateCubeMesh(const Cube& cube)
{
	UE_LOG(LogTemp, Warning, TEXT("GenerateCubeMesh started"));

	TArray<FVector> Vertices;
	Vertices.SetNum(12);
	TArray<int32> Triangles;

	//calculate midpoints
	if (EdgeTable[cube.CubeIndex] & 1)
		Vertices[0] = FVector(cube.Points[0] + cube.Points[1]) * 0.5f * 100;
	if (EdgeTable[cube.CubeIndex] & 2)
		Vertices[1] = FVector(cube.Points[1] + cube.Points[2]) * 0.5f * 100;
	if (EdgeTable[cube.CubeIndex] & 4)
		Vertices[2] = FVector(cube.Points[2] + cube.Points[3]) * 0.5f * 100;
	if (EdgeTable[cube.CubeIndex] & 8)
		Vertices[3] = FVector(cube.Points[3] + cube.Points[0]) * 0.5f * 100;
	if (EdgeTable[cube.CubeIndex] & 16)
		Vertices[4] = FVector(cube.Points[4] + cube.Points[5]) * 0.5f * 100;
	if (EdgeTable[cube.CubeIndex] & 32)
		Vertices[5] = FVector(cube.Points[5] + cube.Points[6]) * 0.5f * 100;
	if (EdgeTable[cube.CubeIndex] & 64)
		Vertices[6] = FVector(cube.Points[6] + cube.Points[7]) * 0.5f * 100;
	if (EdgeTable[cube.CubeIndex] & 128)
		Vertices[7] = FVector(cube.Points[7] + cube.Points[4]) * 0.5f * 100;
	if (EdgeTable[cube.CubeIndex] & 256)
		Vertices[8] = FVector(cube.Points[0] + cube.Points[4]) * 0.5f * 100;
	if (EdgeTable[cube.CubeIndex] & 512)
		Vertices[9] = FVector(cube.Points[1] + cube.Points[5]) * 0.5f * 100;
	if (EdgeTable[cube.CubeIndex] & 1024)
		Vertices[10] = FVector(cube.Points[2] + cube.Points[6]) * 0.5f * 100;
	if (EdgeTable[cube.CubeIndex] & 2048)
		Vertices[11] = FVector(cube.Points[3] + cube.Points[7]) * 0.5f * 100;

	for (int i = 0; TriTable[cube.CubeIndex][i] != -1; i += 3) {
		Triangles.Add(TriTable[cube.CubeIndex][i]);
		Triangles.Add(TriTable[cube.CubeIndex][i + 1]);
		Triangles.Add(TriTable[cube.CubeIndex][i + 2]);
	}



	TArray<FString> VectorStringArray;
	for (const FVector& Element : Vertices)
	{
		VectorStringArray.Add(FString::Printf(TEXT("(%.2f, %.2f, %.2f)"), Element.X, Element.Y, Element.Z));
	}

	// Convert the int32 array elements to strings
	TArray<FString> IntStringArray;
	for (int32 Element : Triangles)
	{
		IntStringArray.Add(FString::Printf(TEXT("%d"), Element));
	}

	// Join the array elements into single strings
	FString JoinedVectorString = FString::Join(VectorStringArray, TEXT(", "));
	FString JoinedIntString = FString::Join(IntStringArray, TEXT(", "));

	// Print the results in UE log
	UE_LOG(LogTemp, Warning, TEXT("Vertices: %s"), *JoinedVectorString);
	UE_LOG(LogTemp, Warning, TEXT("triangles: %s"), *JoinedIntString);


	ProceduralMeshComponent->CreateMeshSection(MeshSectionIndex, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);

	if (MeshMaterial)
	{
		// Create a dynamic material instance from the assigned material
		UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MeshMaterial, this);

		// Check if the dynamic material instance is valid
		if (DynamicMaterialInstance)
		{
			// Set the dynamic material instance on the procedural mesh component
			ProceduralMeshComponent->SetMaterial(MeshSectionIndex, DynamicMaterialInstance);
			FLinearColor RandomColor = FLinearColor::MakeRandomColor();
			DynamicMaterialInstance->SetVectorParameterValue(TEXT("Color"), RandomColor);

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create dynamic material instance!"));
		}
	}

	MeshSectionIndex++;



	// Clear any existing mesh data
	//ProceduralMeshComponent->ClearAllMeshSections();

	// Create a new mesh section
	//ProceduralMeshComponent->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);

	UE_LOG(LogTemp, Warning, TEXT("GenerateCubeMesh finished"));

}

void AMarchingCubesActor::CleanUpMesh()
{
	UE_LOG(LogTemp, Warning, TEXT("Cleanup started"));

	Points.Empty();
	Points.Compact(); // Compact() is not necessary, but it's good practice 

	TArray<AActor*> ChildActors;
	this->GetAttachedActors(ChildActors);

	for (AActor* ChildActor : ChildActors) {
		if (ChildActor) {
			ChildActor->Destroy();
		}
	}

	ProceduralMeshComponent = this->FindComponentByClass<UProceduralMeshComponent>();
	if (!ProceduralMeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ProceduralMeshComponent not found"));
		return;
	}
	ProceduralMeshComponent->ClearAllMeshSections();

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
				Points.Add(FVector(x, y, z), FMath::FRandRange(min, max));
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("%d points generated"), Points.Num());
}

void AMarchingCubesActor::SpawnDebugSphere(const FVector& location, const float& value)
{
	if (DebugSphereActorBlueprint == nullptr)
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

	DebugSphereInstance->SetActorLocation(location * 100);
	DebugSphereInstance->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	DebugSphereInstance->SetValue(value);
}



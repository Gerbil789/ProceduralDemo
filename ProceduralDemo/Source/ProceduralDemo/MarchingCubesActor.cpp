#include "MarchingCubesActor.h"
#include "ProceduralMeshComponent.h"
#include "Math/UnrealMathUtility.h"

AMarchingCubesActor::AMarchingCubesActor()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
	ProceduralMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

AMarchingCubesActor::~AMarchingCubesActor()
{
	//CleanUpMesh();
	UE_LOG(LogTemp, Warning, TEXT("AMarchingCubesActor destroyed"));
}

void AMarchingCubesActor::GenerateMesh(const FIntVector& gridSize, const float& surfaceLevel, const float& offset, const bool& lerp, const bool& debug)
{
	UE_LOG(LogTemp, Warning, TEXT("Generate started"));
	SurfaceLevel = surfaceLevel;
	Offset = offset;
	Debug = debug;
	ChunkOffset = FVector(gridSize.X * Offset / 2, gridSize.Y * Offset / 2, gridSize.Z * Offset / 2);
	CleanUpMesh();

	AllVertices.Empty();
	AllTriangles.Empty();


	if (!ProceduralMeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ProceduralMeshComponent not found"));
		return;
	}

	GeneratePoints(gridSize, 0.0f, 1.0f);

	//Spawn Debug Spheres
	if (Debug) {
		SpawnDebugSpheres();
	}

	for (int32 X = 0; X < gridSize.X - 1; ++X) {
		for (int32 Y = 0; Y < gridSize.Y - 1; ++Y) {
			for (int32 Z = 0; Z < gridSize.Z - 1; ++Z) {
				Cube cube;
				FVector CubeOrigin(X, Y, Z);
				
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
				int32 CubeIndex = 0;
				for (int32 i = 0; i < 8; ++i) {
					if (cube.Values[i] < SurfaceLevel)
						CubeIndex |= 1 << i;
				}

				// Skip the cube if it is entirely inside or outside of the surface
				if (EdgeTable[CubeIndex] == 0) {
					continue;
				}

				GenerateCubeMesh(cube, CubeIndex, lerp);
			}
		}
	}

	ProceduralMeshComponent->CreateMeshSection(0, AllVertices, AllTriangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);


	if (!MeshMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("MeshMaterial not found"));
		return;
	}

	ProceduralMeshComponent->SetMaterial(0, MeshMaterial);

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

void AMarchingCubesActor::GenerateCubeMesh(const Cube& cube, const int32& cubeIndex, const bool& lerp)
{
	TArray<FVector> Vertices;
	Vertices.SetNum(12);

	for (int i = 0; TriTable[cubeIndex][i] != -1; i += 3) {
		AllTriangles.Add(AllVertices.Num() + TriTable[cubeIndex][i]);
		AllTriangles.Add(AllVertices.Num() + TriTable[cubeIndex][i + 1]);
		AllTriangles.Add(AllVertices.Num() + TriTable[cubeIndex][i + 2]);
	}

	

	//use linear interpolation 
	if (lerp) {
		if (EdgeTable[cubeIndex] & 1)
			Vertices[0] = InterpolateVertex(cube.Points[0], cube.Points[1], cube.Values[0], cube.Values[1]) * Offset - ChunkOffset;
		if (EdgeTable[cubeIndex] & 2)
			Vertices[1] = InterpolateVertex(cube.Points[1], cube.Points[2], cube.Values[1], cube.Values[2]) * Offset - ChunkOffset;
		if (EdgeTable[cubeIndex] & 4)
			Vertices[2] = InterpolateVertex(cube.Points[2], cube.Points[3], cube.Values[2], cube.Values[3]) * Offset - ChunkOffset;
		if (EdgeTable[cubeIndex] & 8)
			Vertices[3] = InterpolateVertex(cube.Points[3], cube.Points[0], cube.Values[3], cube.Values[0]) * Offset - ChunkOffset;
		if (EdgeTable[cubeIndex] & 16)
			Vertices[4] = InterpolateVertex(cube.Points[4], cube.Points[5], cube.Values[4], cube.Values[5]) * Offset - ChunkOffset;
		if (EdgeTable[cubeIndex] & 32)
			Vertices[5] = InterpolateVertex(cube.Points[5], cube.Points[6], cube.Values[5], cube.Values[6]) * Offset - ChunkOffset;
		if (EdgeTable[cubeIndex] & 64)
			Vertices[6] = InterpolateVertex(cube.Points[6], cube.Points[7], cube.Values[6], cube.Values[7]) * Offset - ChunkOffset;
		if (EdgeTable[cubeIndex] & 128)
			Vertices[7] = InterpolateVertex(cube.Points[7], cube.Points[4], cube.Values[7], cube.Values[4]) * Offset - ChunkOffset;
		if (EdgeTable[cubeIndex] & 256)
			Vertices[8] = InterpolateVertex(cube.Points[0], cube.Points[4], cube.Values[0], cube.Values[4]) * Offset - ChunkOffset;
		if (EdgeTable[cubeIndex] & 512)
			Vertices[9] = InterpolateVertex(cube.Points[1], cube.Points[5], cube.Values[1], cube.Values[5]) * Offset - ChunkOffset;
		if (EdgeTable[cubeIndex] & 1024)
			Vertices[10] = InterpolateVertex(cube.Points[2], cube.Points[6], cube.Values[2], cube.Values[6]) * Offset - ChunkOffset;
		if (EdgeTable[cubeIndex] & 2048)
			Vertices[11] = InterpolateVertex(cube.Points[3], cube.Points[7], cube.Values[3], cube.Values[7]) * Offset - ChunkOffset;
	}
	else {
		//use midpoint interpolation
		if (EdgeTable[cubeIndex] & 1)
			Vertices[0] = (FVector(cube.Points[0] + cube.Points[1]) * 0.5f * Offset) - ChunkOffset;
		if (EdgeTable[cubeIndex] & 2)
			Vertices[1] = (FVector(cube.Points[1] + cube.Points[2]) * 0.5f * Offset) - ChunkOffset;
		if (EdgeTable[cubeIndex] & 4)
			Vertices[2] = (FVector(cube.Points[2] + cube.Points[3]) * 0.5f * Offset) - ChunkOffset;
		if (EdgeTable[cubeIndex] & 8)
			Vertices[3] = (FVector(cube.Points[3] + cube.Points[0]) * 0.5f * Offset) - ChunkOffset;
		if (EdgeTable[cubeIndex] & 16)
			Vertices[4] = (FVector(cube.Points[4] + cube.Points[5]) * 0.5f * Offset) - ChunkOffset;
		if (EdgeTable[cubeIndex] & 32)
			Vertices[5] = (FVector(cube.Points[5] + cube.Points[6]) * 0.5f * Offset) - ChunkOffset;
		if (EdgeTable[cubeIndex] & 64)
			Vertices[6] = (FVector(cube.Points[6] + cube.Points[7]) * 0.5f * Offset) - ChunkOffset;
		if (EdgeTable[cubeIndex] & 128)
			Vertices[7] = (FVector(cube.Points[7] + cube.Points[4]) * 0.5f * Offset) - ChunkOffset;
		if (EdgeTable[cubeIndex] & 256)
			Vertices[8] = (FVector(cube.Points[0] + cube.Points[4]) * 0.5f * Offset) - ChunkOffset;
		if (EdgeTable[cubeIndex] & 512)
			Vertices[9] = (FVector(cube.Points[1] + cube.Points[5]) * 0.5f * Offset) - ChunkOffset;
		if (EdgeTable[cubeIndex] & 1024)
			Vertices[10] = (FVector(cube.Points[2] + cube.Points[6]) * 0.5f * Offset) - ChunkOffset;
		if (EdgeTable[cubeIndex] & 2048)
			Vertices[11] = (FVector(cube.Points[3] + cube.Points[7]) * 0.5f * Offset) - ChunkOffset;
	}


	AllVertices.Append(Vertices);
}

void AMarchingCubesActor::CleanUpMesh()
{
	UE_LOG(LogTemp, Warning, TEXT("Cleanup started"));

	Points.Empty();

	TArray<AActor*> ChildActors;
	this->GetAttachedActors(ChildActors);

	for (AActor* ChildActor : ChildActors) {
		if (ChildActor) {
			ChildActor->Destroy();
		}
	}

	if (!ProceduralMeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ProceduralMeshComponent not found"));
		return;
	}
	ProceduralMeshComponent->ClearAllMeshSections();

	UE_LOG(LogTemp, Warning, TEXT("Cleanup finished"));
}

void AMarchingCubesActor::GeneratePoints(const FIntVector& gridSize, float min, float max)
{
	UE_LOG(LogTemp, Warning, TEXT("GeneratePoints started"));

	if (gridSize.X <= 0 || gridSize.Y <= 0 || gridSize.Z <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid size"));
		return;
	}
	FVector ChunkLocation = GetActorLocation() / Offset; // Convert to grid space

	for (int32 X = 0; X < gridSize.X; X++) {
		for (int32 Y = 0; Y < gridSize.Y; Y++) {
			for (int32 Z = 0; Z < gridSize.Z; Z++)
			{
				FVector GridPoint(X, Y, Z);

				FVector WorldPoint = ChunkLocation + GridPoint;

				float PerlinNoiseValue = FMath::PerlinNoise3D(WorldPoint / 25.0f);
				float RemappedValue = FMath::GetMappedRangeValueClamped(FVector2D(-1.0f, 1.0f), FVector2D(min, max), PerlinNoiseValue);

				Points.Add(GridPoint, RemappedValue);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("%d points generated"), Points.Num());
}

void AMarchingCubesActor::SpawnDebugSpheres()
{
	for (const auto& point : Points) {
		FVector pointLocation = point.Key;
		float pointColorFactor = point.Value;
		SpawnDebugSphere(point.Key, point.Value);
	}
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

	DebugSphereInstance->SetActorLocation(location * Offset);
	DebugSphereInstance->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	DebugSphereInstance->SetValue(value);
}

FVector AMarchingCubesActor::InterpolateVertex(const FVector& pointA, const FVector& pointB, const float& valueA, const float& valueB)
{

	if(FMath::Abs(SurfaceLevel - valueA) < 0.00001f)
		return pointA;
	if (FMath::Abs(SurfaceLevel - valueB) < 0.00001f)
		return pointB;
	if(FMath::Abs(valueA - valueB) < 0.00001f)
		return pointA;


	FVector interpolatedVertex = FVector::ZeroVector;
	float t = (SurfaceLevel - valueA) / (valueB - valueA);
	interpolatedVertex.X = FMath::Lerp(pointA.X, pointB.X, t);
	interpolatedVertex.Y = FMath::Lerp(pointA.Y, pointB.Y, t);
	interpolatedVertex.Z = FMath::Lerp(pointA.Z, pointB.Z, t);
	return interpolatedVertex;
}



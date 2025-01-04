#include "Chunk.h"
#include "ProceduralMeshComponent.h"
#include "Math/UnrealMathUtility.h"

AChunk::AChunk()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
	ProceduralMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

}

void AChunk::GenerateMesh(const bool& smooth, const bool& normals)
{
	if (!ProceduralMeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ProceduralMeshComponent not found"));
		return;
	}
	CleanUpMesh();

	MeshData Mesh;
	MarchingCubesAlgorithm* MarchingCubes = new MarchingCubesAlgorithm();

	Mesh = MarchingCubes->GenerateMesh(GetActorLocation(), FIntVector(10, 10, 10), 0.48f, 500.0f, smooth);
	TArray<FVector>& Vertices = Mesh.Vertices;
	TArray<int32>& Triangles = Mesh.Triangles;
	TArray<FVector> Normals;

	if (normals) {
		Normals = Mesh.Normals;
	}
	else {
		Normals.Empty();
	}

	UE_LOG(LogTemp, Warning, TEXT("Mesh.Vertices.Num() = %d"), Vertices.Num());
	UE_LOG(LogTemp, Warning, TEXT("Mesh.Triangles.Num() = %d"), Triangles.Num());
	UE_LOG(LogTemp, Warning, TEXT("Mesh.Normals.Num() = %d"), Normals.Num());

	delete MarchingCubes;

	ProceduralMeshComponent->CreateMeshSection(0, Vertices, Triangles, Normals, TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	if (!MeshMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("MeshMaterial not found"));
	}
	else {
		ProceduralMeshComponent->SetMaterial(0, MeshMaterial);
	}

	//UE_LOG(LogTemp, Warning, TEXT("Generate finished"));
}

void AChunk::CleanUpMesh()
{
	ProceduralMeshComponent->ClearAllMeshSections();
}	



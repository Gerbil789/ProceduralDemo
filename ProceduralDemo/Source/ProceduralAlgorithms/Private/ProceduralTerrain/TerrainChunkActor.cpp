#include "ProceduralTerrain/TerrainChunkActor.h"
#include "ProceduralTerrain/InfiniteTerrain.h"

ATerrainChunkActor::ATerrainChunkActor()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
	RootComponent = MeshComponent;
}



void ATerrainChunkActor::GenerateMesh(int X, int Y, AInfiniteTerrain* InfiniteTerrain)
{
	SetActorLocation(FVector(X * InfiniteTerrain->ChunkSize, Y * InfiniteTerrain->ChunkSize, 0));

  TArray<FVector> Vertices;
  TArray<int32> Triangles;
  TArray<FVector2D> UVs;
  TArray<FVector> Normals;
  TArray<FProcMeshTangent> Tangents;

  for (int y = 0; y <= InfiniteTerrain->ChunkSize; y++)
  {
    for (int x = 0; x <= InfiniteTerrain->ChunkSize; x++)
    {
			float Height = InfiniteTerrain->CalculateHeight(x, y);
      Vertices.Add(FVector(x, y, Height));

      UVs.Add(FVector2D(x / (float)InfiniteTerrain->ChunkSize, y / (float)InfiniteTerrain->ChunkSize));

      if (x < InfiniteTerrain->ChunkSize && y < InfiniteTerrain->ChunkSize)
      {
        int i = x + y * (InfiniteTerrain->ChunkSize + 1);
        Triangles.Add(i);
        Triangles.Add(i + InfiniteTerrain->ChunkSize + 1);
        Triangles.Add(i + 1);

        Triangles.Add(i + 1);
        Triangles.Add(i + InfiniteTerrain->ChunkSize + 1);
        Triangles.Add(i + InfiniteTerrain->ChunkSize + 2);
      }
    }
  }

  MeshComponent->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, TArray<FColor>(), Tangents, true);
}




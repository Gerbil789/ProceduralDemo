#include "ProceduralTerrain.h"

AProceduralTerrain::AProceduralTerrain()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	RootComponent = Mesh;
}

int32 AProceduralTerrain::GenerateChunk(int32 ChunkX, int32 ChunkY)
{
  if (!Mesh)
  {
    UE_LOG(LogTemp, Error, TEXT("ProceduralMeshComponent is null!"));
    return 0;
  }

  TArray<FVector> Vertices;
  TArray<int32> Triangles;
  TArray<FVector2D> UVs;
  TArray<FVector> Normals;
  TArray<FProcMeshTangent> Tangents;

  float StartX = ChunkX * (ChunkSize - 1) * QuadSize;
  float StartY = ChunkY * (ChunkSize - 1) * QuadSize;


  for (int32 y = 0; y < ChunkSize; y++)
  {
    for (int32 x = 0; x < ChunkSize; x++)
    {
      float WorldX = StartX + x * QuadSize;
      float WorldY = StartY + y * QuadSize;

			float Height = FMath::PerlinNoise2D(FVector2D(WorldX, WorldY) * Frequency) * Amplitude;
			Height += FMath::PerlinNoise2D(FVector2D(WorldX, WorldY) * Frequency * 2) * Amplitude / 2;
			Height += FMath::PerlinNoise2D(FVector2D(WorldX, WorldY) * Frequency * 4) * Amplitude / 4;
      Vertices.Add(FVector(WorldX, WorldY, Height));

      if (x < ChunkSize - 1 && y < ChunkSize - 1)
      {
        int32 i = y * ChunkSize + x;
        Triangles.Add(i);
        Triangles.Add(i + ChunkSize);
        Triangles.Add(i + 1);
        Triangles.Add(i + ChunkSize);
        Triangles.Add(i + ChunkSize + 1);
        Triangles.Add(i + 1);
      }

      UVs.Add(FVector2D(x / (float)(ChunkSize - 1), y / (float)(ChunkSize - 1)));
      Normals.Add(FVector(0, 0, 1));
      Tangents.Add(FProcMeshTangent(1, 0, 0));
    }
  }

  UE_LOG(LogTemp, Warning, TEXT("Creating chunk: %d, %d | Vertex Count: %d | Triangle Count: %d"), ChunkX, ChunkY, Vertices.Num(), Triangles.Num());

	int32 SectionIndex = FMath::Rand32() % 10000; //TODO: Find a better way to generate unique section index
  Mesh->CreateMeshSection(SectionIndex, Vertices, Triangles, Normals, UVs, TArray<FColor>(), Tangents, true);
  if (TerrainMaterial)
  {
    Mesh->SetMaterial(SectionIndex, TerrainMaterial);
  }

	return SectionIndex;
}
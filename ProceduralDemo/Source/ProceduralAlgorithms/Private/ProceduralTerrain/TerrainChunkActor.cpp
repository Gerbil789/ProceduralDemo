#include "ProceduralTerrain/TerrainChunkActor.h"
#include "ProceduralTerrain/InfiniteTerrain.h"

ATerrainChunkActor::ATerrainChunkActor()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
	RootComponent = MeshComponent;
}

void ATerrainChunkActor::GenerateMesh(FIntPoint ChunkCoordinates, AInfiniteTerrain* Terrain)
{
 
  // Step 1: Prepare HeightMap
  TArray<float> HeightMap;
  HeightMap.SetNum((Terrain->ChunkSize + 1) * (Terrain->ChunkSize + 1));

  // Step 2: Initialize HeightMap with zeros
  for (int i = 0; i < HeightMap.Num(); i++)
  {
    HeightMap[i] = 0.0f;
  }

  // Step 3: Apply all modifiers
  Terrain->ApplyModifiers(ChunkCoordinates, HeightMap);

  // Step 4: Convert HeightMap into procedural mesh data
  TArray<FVector> Vertices;
  TArray<int32> Triangles;
  TArray<FVector2D> UVs;
  TArray<FVector> Normals;
  TArray<FProcMeshTangent> Tangents;

  for (int y = 0; y <= Terrain->ChunkSize; y++)
  {
    for (int x = 0; x <= Terrain->ChunkSize; x++)
    {
      int Index = x + y * (Terrain->ChunkSize + 1);
      float Height = HeightMap[Index];

      Vertices.Add(FVector(x * Terrain->QuadSize, y * Terrain->QuadSize, Height));
      UVs.Add(FVector2D(x / (float)Terrain->ChunkSize, y / (float)Terrain->ChunkSize));

      if (x < Terrain->ChunkSize && y < Terrain->ChunkSize)
      {
        int i = x + y * (Terrain->ChunkSize + 1);
        Triangles.Add(i);
        Triangles.Add(i + Terrain->ChunkSize + 1);
        Triangles.Add(i + 1);

        Triangles.Add(i + 1);
        Triangles.Add(i + Terrain->ChunkSize + 1);
        Triangles.Add(i + Terrain->ChunkSize + 2);
      }
    }
  }

  MeshComponent->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, TArray<FColor>(), Tangents, true);
  MeshComponent->SetMaterial(0, Terrain->TerrainMaterial);

}





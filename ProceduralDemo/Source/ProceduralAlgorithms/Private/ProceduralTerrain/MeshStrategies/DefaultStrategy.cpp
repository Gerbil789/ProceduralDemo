#include "ProceduralTerrain/MeshStrategies/DefaultStrategy.h"

void DefaultStrategy::GenerateMesh(const TArray<float>& HeightMap, int ChunkSize, int QuadSize, FMeshData& MeshData)
{
  for (int y = 0; y <= ChunkSize; y++)
  {
    for (int x = 0; x <= ChunkSize; x++)
    {
      int Index = x + y * (ChunkSize + 1);
      float Height = HeightMap[Index];

      MeshData.Vertices.Add(FVector(x * QuadSize, y * QuadSize, Height));
      MeshData.UVs.Add(FVector2D(x / (float)ChunkSize, y / (float)ChunkSize));

      if (x < ChunkSize && y < ChunkSize)
      {
        int i = x + y * (ChunkSize + 1);
        MeshData.Triangles.Add(i);
        MeshData.Triangles.Add(i + ChunkSize + 1);
        MeshData.Triangles.Add(i + 1);

        MeshData.Triangles.Add(i + 1);
        MeshData.Triangles.Add(i + ChunkSize + 1);
        MeshData.Triangles.Add(i + ChunkSize + 2);
      }
    }
  }
}

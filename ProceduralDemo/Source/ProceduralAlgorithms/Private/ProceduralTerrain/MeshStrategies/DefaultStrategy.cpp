#include "ProceduralTerrain/MeshStrategies/DefaultStrategy.h"

void DefaultStrategy::CalculateNormals(FMeshData& MeshData)
{
  MeshData.Normals.Empty();
  MeshData.Normals.AddZeroed(MeshData.Vertices.Num());

  // For each triangle, calculate the face normal and add it to all three vertices
  for (int32 i = 0; i < MeshData.Triangles.Num(); i += 3)
  {
    int32 Index0 = MeshData.Triangles[i];
    int32 Index1 = MeshData.Triangles[i + 1];
    int32 Index2 = MeshData.Triangles[i + 2];

    FVector Vertex0 = MeshData.Vertices[Index0];
    FVector Vertex1 = MeshData.Vertices[Index1];
    FVector Vertex2 = MeshData.Vertices[Index2];

    FVector Edge1 = Vertex1 - Vertex0;
    FVector Edge2 = Vertex2 - Vertex0;

    // Calculate normal using the correct order for your winding
    // Note we're using Edge1 x Edge2 then inverting the result
    FVector Normal = -FVector::CrossProduct(Edge1, Edge2).GetSafeNormal();

    MeshData.Normals[Index0] += Normal;
    MeshData.Normals[Index1] += Normal;
    MeshData.Normals[Index2] += Normal;
  }

  // Normalize all normals
  for (FVector& Normal : MeshData.Normals)
  {
    Normal.Normalize();
  }

  // Optional: Simple average smoothing
  //TArray<FVector> SmoothedNormals;
  //SmoothedNormals.AddZeroed(MeshData.Vertices.Num());

  //for (int32 i = 0; i < MeshData.Triangles.Num(); i += 3)
  //{
  //  int32 Index0 = MeshData.Triangles[i];
  //  int32 Index1 = MeshData.Triangles[i + 1];
  //  int32 Index2 = MeshData.Triangles[i + 2];

  //  SmoothedNormals[Index0] += MeshData.Normals[Index0];
  //  SmoothedNormals[Index1] += MeshData.Normals[Index1];
  //  SmoothedNormals[Index2] += MeshData.Normals[Index2];
  //}

  //for (FVector& Normal : SmoothedNormals)
  //{
  //  Normal.Normalize();
  //}

  //MeshData.Normals = SmoothedNormals;
}


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

  CalculateNormals(MeshData);
}

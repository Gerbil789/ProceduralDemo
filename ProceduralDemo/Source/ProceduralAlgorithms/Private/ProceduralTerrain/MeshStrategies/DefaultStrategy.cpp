#include "ProceduralTerrain/MeshStrategies/DefaultStrategy.h"



void DefaultStrategy::GenerateMesh(const TArray<float>& HeightMap, int ChunkSize, int QuadSize, FMeshData& MeshData, bool bGenerateSkirt, float SkirtDepth)
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

  if(bGenerateSkirt)
  {
    GenerateSkirt(MeshData, ChunkSize, SkirtDepth);
  }

}

void DefaultStrategy::GenerateSkirt(FMeshData& MeshData, int ChunkSize, float SkirtDepth)
{
  // Create a separate skirt mesh data
  FMeshData SkirtData;

  // We'll create vertices for each edge, including corners
  // Order: bottom edge (left to right), right edge (bottom to top),
  // top edge (right to left), left edge (top to bottom)

  // Bottom edge vertices (y = 0)
  for (int x = 0; x <= ChunkSize; x++)
  {
    int Index = x + 0 * (ChunkSize + 1);
    FVector OriginalVertex = MeshData.Vertices[Index];

    // Add top vertex (original edge)
    SkirtData.Vertices.Add(OriginalVertex);
    SkirtData.UVs.Add(FVector2D(x / (float)ChunkSize, 0));

    // Add bottom vertex (skirt extension)
    SkirtData.Vertices.Add(FVector(OriginalVertex.X, OriginalVertex.Y, OriginalVertex.Z - SkirtDepth));
    SkirtData.UVs.Add(FVector2D(x / (float)ChunkSize, 1));
  }

  // Right edge vertices (x = ChunkSize)
  for (int y = 1; y <= ChunkSize; y++) // Start at 1 to avoid duplicating bottom corner
  {
    int Index = ChunkSize + y * (ChunkSize + 1);
    FVector OriginalVertex = MeshData.Vertices[Index];

    SkirtData.Vertices.Add(OriginalVertex);
    SkirtData.UVs.Add(FVector2D(y / (float)ChunkSize, 0));

    SkirtData.Vertices.Add(FVector(OriginalVertex.X, OriginalVertex.Y, OriginalVertex.Z - SkirtDepth));
    SkirtData.UVs.Add(FVector2D(y / (float)ChunkSize, 1));
  }

  // Top edge vertices (y = ChunkSize)
  for (int x = ChunkSize - 1; x >= 0; x--) // Start at ChunkSize-1 to avoid duplicating right corner
  {
    int Index = x + ChunkSize * (ChunkSize + 1);
    FVector OriginalVertex = MeshData.Vertices[Index];

    SkirtData.Vertices.Add(OriginalVertex);
    SkirtData.UVs.Add(FVector2D(x / (float)ChunkSize, 0));

    SkirtData.Vertices.Add(FVector(OriginalVertex.X, OriginalVertex.Y, OriginalVertex.Z - SkirtDepth));
    SkirtData.UVs.Add(FVector2D(x / (float)ChunkSize, 1));
  }

  // Left edge vertices (x = 0)
  for (int y = ChunkSize - 1; y >= 1; y--) // Start at ChunkSize-1 to avoid duplicating top corner
  {
    int Index = 0 + y * (ChunkSize + 1);
    FVector OriginalVertex = MeshData.Vertices[Index];

    SkirtData.Vertices.Add(OriginalVertex);
    SkirtData.UVs.Add(FVector2D(y / (float)ChunkSize, 0));

    SkirtData.Vertices.Add(FVector(OriginalVertex.X, OriginalVertex.Y, OriginalVertex.Z - SkirtDepth));
    SkirtData.UVs.Add(FVector2D(y / (float)ChunkSize, 1));
  }

  // Generate triangles for the skirt with CORRECT WINDING ORDER
  int TotalEdgeVertices = SkirtData.Vertices.Num();
  int Quads = (TotalEdgeVertices / 2) - 1;

  for (int i = 0; i < Quads; i++)
  {
    int topLeft = i * 2;
    int topRight = topLeft + 2;
    int bottomLeft = topLeft + 1;
    int bottomRight = topRight + 1;

    // Correct winding: (topLeft, topRight, bottomLeft)
    SkirtData.Triangles.Add(topLeft);
    SkirtData.Triangles.Add(topRight);
    SkirtData.Triangles.Add(bottomLeft);

    // Correct winding: (bottomLeft, topRight, bottomRight)
    SkirtData.Triangles.Add(bottomLeft);
    SkirtData.Triangles.Add(topRight);
    SkirtData.Triangles.Add(bottomRight);
  }

  // Close the loop between last and first quad
  int lastTop = TotalEdgeVertices - 2;
  int lastBottom = TotalEdgeVertices - 1;
  int firstTop = 0;
  int firstBottom = 1;

  SkirtData.Triangles.Add(lastTop);
  SkirtData.Triangles.Add(firstTop);
  SkirtData.Triangles.Add(lastBottom);

  SkirtData.Triangles.Add(lastBottom);
  SkirtData.Triangles.Add(firstTop);
  SkirtData.Triangles.Add(firstBottom);

  // Calculate normals for the skirt
  CalculateNormals(SkirtData);

  // Append skirt data to main mesh data
  int VertexOffset = MeshData.Vertices.Num();

  // Add vertices
  MeshData.Vertices.Append(SkirtData.Vertices);
  MeshData.UVs.Append(SkirtData.UVs);
  MeshData.Normals.Append(SkirtData.Normals);

  // Add triangles with offset
  for (int32 Index : SkirtData.Triangles)
  {
    MeshData.Triangles.Add(Index + VertexOffset);
  }
}


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
}


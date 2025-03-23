#include "ProceduralTerrain/MeshStrategies/VertexClusteringStrategy.h"

void VertexClusteringStrategy::GenerateMesh(const TArray<float>& HeightMap, int ChunkSize, int QuadSize, int DecimationFactor, FMeshData& MeshData)
{
  if(DecimationFactor <= 0)
  {
    UE_LOG(LogTemp, Error, TEXT("Decimation factor must be greater than 0"));
    return;
  }

  int ClusterSize = DecimationFactor; // Define decimation intensity
  int NumClustersX = ChunkSize / ClusterSize;
  int NumClustersY = ChunkSize / ClusterSize;

  for (int y = 0; y < NumClustersY; y++)
  {
    for (int x = 0; x < NumClustersX; x++)
    {
      FVector SumPosition = FVector::ZeroVector;
      int VertexCount = 0;

      // Loop through vertices in this cluster
      for (int dy = 0; dy < ClusterSize; dy++)
      {
        for (int dx = 0; dx < ClusterSize; dx++)
        {
          int LocalX = x * ClusterSize + dx;
          int LocalY = y * ClusterSize + dy;
          if (LocalX >= ChunkSize || LocalY >= ChunkSize) continue;

          int Index = LocalX + LocalY * (ChunkSize + 1);
          float Height = HeightMap[Index];
          FVector Position = FVector(LocalX * QuadSize, LocalY * QuadSize, Height);

          SumPosition += Position;
          VertexCount++;
        }
      }

      // Create the centroid vertex
      FVector Centroid = SumPosition / VertexCount;
      int VertexIndex = MeshData.Vertices.Num();
      MeshData.Vertices.Add(Centroid);
      MeshData.UVs.Add(FVector2D(x / (float)NumClustersX, y / (float)NumClustersY));

      // Create triangles
      if (x < NumClustersX - 1 && y < NumClustersY - 1)
      {
        int i = VertexIndex;
        MeshData.Triangles.Add(i);
        MeshData.Triangles.Add(i + NumClustersX);
        MeshData.Triangles.Add(i + 1);


        MeshData.Triangles.Add(i + 1);
        MeshData.Triangles.Add(i + NumClustersX);
        MeshData.Triangles.Add(i + NumClustersX + 1);

      }
    }
  }
}

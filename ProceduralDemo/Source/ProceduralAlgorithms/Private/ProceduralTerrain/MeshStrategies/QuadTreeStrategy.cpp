#include "ProceduralTerrain/MeshStrategies/QuadTreeStrategy.h"


struct FQuadTreeNode
{
  FIntRect Bounds; // The bounds of the node in the heightmap
  TArray<float> Heights; // Height values within this node
  TArray<FQuadTreeNode*> Children; // Child nodes (if subdivided)

  FQuadTreeNode(const FIntRect& InBounds) : Bounds(InBounds) {}
  ~FQuadTreeNode()
  {
    for (auto Child : Children)
    {
      delete Child;
    }
  }
};

void SubdivideQuadTreeNode(FQuadTreeNode* Node, const TArray<float>& HeightMap, int ChunkSize, float HeightVariationThreshold)
{
  if (Node->Bounds.Width() <= 1 || Node->Bounds.Height() <= 1) return; // Stop subdividing if the node is too small

  // Calculate height variation within the node
  float MinHeight = FLT_MAX;
  float MaxHeight = -FLT_MAX;
  for (int y = Node->Bounds.Min.Y; y <= Node->Bounds.Max.Y; y++)
  {
    for (int x = Node->Bounds.Min.X; x <= Node->Bounds.Max.X; x++)
    {
      float Height = HeightMap[x + y * (ChunkSize + 1)];
      MinHeight = FMath::Min(MinHeight, Height);
      MaxHeight = FMath::Max(MaxHeight, Height);
    }
  }

  float HeightVariation = MaxHeight - MinHeight;
  if (HeightVariation <= HeightVariationThreshold) return; // Stop subdividing if the height variation is below the threshold

  // Subdivide the node into 4 children
  int HalfWidth = Node->Bounds.Width() / 2;
  int HalfHeight = Node->Bounds.Height() / 2;

  Node->Children.Add(new FQuadTreeNode(FIntRect(Node->Bounds.Min, FIntPoint(Node->Bounds.Min.X + HalfWidth, Node->Bounds.Min.Y + HalfHeight))));
  Node->Children.Add(new FQuadTreeNode(FIntRect(FIntPoint(Node->Bounds.Min.X + HalfWidth, Node->Bounds.Min.Y), FIntPoint(Node->Bounds.Max.X, Node->Bounds.Min.Y + HalfHeight))));
  Node->Children.Add(new FQuadTreeNode(FIntRect(FIntPoint(Node->Bounds.Min.X, Node->Bounds.Min.Y + HalfHeight), FIntPoint(Node->Bounds.Min.X + HalfWidth, Node->Bounds.Max.Y))));
  Node->Children.Add(new FQuadTreeNode(FIntRect(FIntPoint(Node->Bounds.Min.X + HalfWidth, Node->Bounds.Min.Y + HalfHeight), Node->Bounds.Max)));

  // Recursively subdivide children
  for (auto Child : Node->Children)
  {
    SubdivideQuadTreeNode(Child, HeightMap, ChunkSize, HeightVariationThreshold);
  }
}

void GenerateMeshFromQuadTreeNode(FQuadTreeNode* Node, const TArray<float>& HeightMap, TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector2D>& UVs, int ChunkSize, int QuadSize)
{
  if (Node->Children.IsEmpty())
  {
    // Leaf node: create a single quad for this node
    int Index = Vertices.Num();

    Vertices.Add(FVector(Node->Bounds.Min.X * QuadSize, Node->Bounds.Min.Y * QuadSize, HeightMap[Node->Bounds.Min.X + Node->Bounds.Min.Y * (ChunkSize + 1)]));
    Vertices.Add(FVector(Node->Bounds.Max.X * QuadSize, Node->Bounds.Min.Y * QuadSize, HeightMap[Node->Bounds.Max.X + Node->Bounds.Min.Y * (ChunkSize + 1)]));
    Vertices.Add(FVector(Node->Bounds.Min.X * QuadSize, Node->Bounds.Max.Y * QuadSize, HeightMap[Node->Bounds.Min.X + Node->Bounds.Max.Y * (ChunkSize + 1)]));
    Vertices.Add(FVector(Node->Bounds.Max.X * QuadSize, Node->Bounds.Max.Y * QuadSize, HeightMap[Node->Bounds.Max.X + Node->Bounds.Max.Y * (ChunkSize + 1)]));

    UVs.Add(FVector2D(Node->Bounds.Min.X / (float)ChunkSize, Node->Bounds.Min.Y / (float)ChunkSize));
    UVs.Add(FVector2D(Node->Bounds.Max.X / (float)ChunkSize, Node->Bounds.Min.Y / (float)ChunkSize));
    UVs.Add(FVector2D(Node->Bounds.Min.X / (float)ChunkSize, Node->Bounds.Max.Y / (float)ChunkSize));
    UVs.Add(FVector2D(Node->Bounds.Max.X / (float)ChunkSize, Node->Bounds.Max.Y / (float)ChunkSize));

    Triangles.Add(Index);
    Triangles.Add(Index + 2);
    Triangles.Add(Index + 1);

    Triangles.Add(Index + 1);
    Triangles.Add(Index + 2);
    Triangles.Add(Index + 3);
  }
  else
  {
    // Recursively generate mesh for children
    for (auto Child : Node->Children)
    {
      GenerateMeshFromQuadTreeNode(Child, HeightMap, Vertices, Triangles, UVs, ChunkSize, QuadSize);
    }
  }
}

void QuadTreeStrategy::GenerateMesh(const TArray<float>& HeightMap, int ChunkSize, int QuadSize, float HeightThreshold, FMeshData& MeshData)
{
  FQuadTreeNode* RootNode = new FQuadTreeNode(FIntRect(FIntPoint(0, 0), FIntPoint(ChunkSize, ChunkSize)));

  // Subdivide the quadtree
  float HeightVariationThreshold = 50.0f; // Adjust this threshold as needed
  SubdivideQuadTreeNode(RootNode, HeightMap, ChunkSize, HeightThreshold);

  // Generate mesh from the quadtree
  GenerateMeshFromQuadTreeNode(RootNode, HeightMap, MeshData.Vertices, MeshData.Triangles, MeshData.UVs, ChunkSize, QuadSize);

  // Clean up the quadtree
  delete RootNode;

	// Calculate normals
	CalculateNormals(MeshData);
}

void QuadTreeStrategy::CalculateNormals(FMeshData& MeshData)
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

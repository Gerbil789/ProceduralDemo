#pragma once

#include "CoreMinimal.h"

struct Cube
{
	FVector Points[8];
	float Values[8];
};

struct MeshData
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
};

class PROCEDURALDEMO_API MarchingCubesAlgorithm
{
public:
	MeshData GenerateMesh(const FVector& chunkLocation, const FIntVector& gridSize, const float& surfaceLevel = 0.5f, const float& offset = 100.0f, const bool& lerp = false);
private:
	void GeneratePoints(const FIntVector& gridSize, TMap<FVector, float>& points);
	void GenerateCubeMesh(const Cube& cube, const int32& cubeIndex, MeshData& meshData);
	FVector InterpolateVertex(const FVector& pointA, const FVector& pointB, const float& valueA, const float& valueB);

	static const FVector CornerOffsets[8];

	static const int EdgeTable[256];

	static const int32 TriTable[256][16];

	FVector ChunkOffset;
	FVector ChunkLocation;
	float SurfaceLevel;
	float Offset;
	bool Lerp;
};

#pragma once

#include "ProceduralTerrain/Structs/MeshData.h"

class PROCEDURALALGORITHMS_API DefaultStrategy
{
public:
	static void GenerateMesh(const TArray<float>& HeightMap, int ChunkSize, int QuadSize, FMeshData& MeshData, bool bGenerateSkirt, float SkirtDepth = 0.0f);
private:
	static void GenerateSkirt(FMeshData& MeshData, int ChunkSize, float SkirtDepth);
	static void CalculateNormals(FMeshData& MeshData);
};

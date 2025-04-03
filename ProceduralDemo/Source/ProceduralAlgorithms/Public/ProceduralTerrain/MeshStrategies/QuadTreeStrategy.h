#pragma once

#include "ProceduralTerrain/Structs/MeshData.h"

class PROCEDURALALGORITHMS_API QuadTreeStrategy
{
public:
	static void GenerateMesh(const TArray<float>& HeightMap, int ChunkSize, int QuadSize, float HeightThreshold, FMeshData& MeshData);
private:
	static void CalculateNormals(FMeshData& MeshData);
};

#pragma once

#include "ProceduralTerrain/Structs/MeshData.h"

class PROCEDURALALGORITHMS_API VertexClusteringStrategy
{
public:
	static void GenerateMesh(const TArray<float>& HeightMap, int ChunkSize, int QuadSize, int DecimationFactor, FMeshData& MeshData);

};

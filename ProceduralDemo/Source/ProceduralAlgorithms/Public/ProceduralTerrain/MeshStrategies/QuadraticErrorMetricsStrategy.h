#pragma once

#include "ProceduralTerrain/Structs/MeshData.h"

class PROCEDURALALGORITHMS_API QuadraticErrorMetricsStrategy
{
public:
	static void GenerateMesh(const TArray<float>& HeightMap, int ChunkSize, int QuadSize, int Threshold, FMeshData& MeshData);
};

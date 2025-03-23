#pragma once

#include "ProceduralTerrain/Structs/MeshData.h"

class PROCEDURALALGORITHMS_API DefaultStrategy
{
public:
	static void GenerateMesh(const TArray<float>& HeightMap, int ChunkSize, int QuadSize, FMeshData& MeshData);

};

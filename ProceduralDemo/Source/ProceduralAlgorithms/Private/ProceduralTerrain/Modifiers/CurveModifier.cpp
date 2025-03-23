#include "ProceduralTerrain/Modifiers/CurveModifier.h"

void UCurveModifier::ApplyModifier(TArray<float>& HeightMap, int ChunkSize, FVector2D ChunkCoordinates) const
{
	if (!Curve || HeightMap.IsEmpty()) return;

	for (int y = 0; y <= ChunkSize; y++)
	{
		for (int x = 0; x <= ChunkSize; x++)
		{
			int Index = x + y * (ChunkSize + 1);
			HeightMap[Index] *= Curve->GetFloatValue(HeightMap[Index]);
		}
	}
}


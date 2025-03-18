#include "ProceduralTerrain/Modifiers/PerlinNoiseModifier.h"

void UPerlinNoiseModifier::ApplyModifier(TArray<float>& HeightMap, int ChunkSize, FVector2D ChunkCoordinates) const
{
  if (HeightMap.IsEmpty()) return;

	TArray<float> NewHeightMap;
	NewHeightMap.SetNum((ChunkSize + 1) * (ChunkSize + 1));

  for (int y = 0; y <= ChunkSize; y++)
  {
    for (int x = 0; x <= ChunkSize; x++)
    {
      int Index = x + y * (ChunkSize + 1);
      FVector2D WorldPos = ChunkCoordinates + FVector2D(x, y);
      float NoiseValue = FMath::PerlinNoise2D(WorldPos / Scale);

			if (Curve)
			{
				NoiseValue = Curve->GetFloatValue(NoiseValue);
			}

			NoiseValue *= Amplitude;

      NewHeightMap[Index] = NoiseValue;
    }
  }

  if (ModifierType == ETerrainModifierType::Additive)
  {
		for (int y = 0; y <= ChunkSize; y++)
		{
			for (int x = 0; x <= ChunkSize; x++)
			{
				int Index = x + y * (ChunkSize + 1);
				HeightMap[Index] += NewHeightMap[Index];
			}
		}
  }
  else if (ModifierType == ETerrainModifierType::Multiplicative)
  {
		for (int y = 0; y <= ChunkSize; y++)
		{
			for (int x = 0; x <= ChunkSize; x++)
			{
				int Index = x + y * (ChunkSize + 1);
				HeightMap[Index] *= NewHeightMap[Index];
			}
		}
  }
}

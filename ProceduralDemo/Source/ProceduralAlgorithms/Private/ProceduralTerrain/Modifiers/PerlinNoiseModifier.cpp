#include "ProceduralTerrain/Modifiers/PerlinNoiseModifier.h"

//float UPerlinNoiseModifier::GetHeight_Implementation(FVector2D WorldLocation) const
//{
//  return FMath::PerlinNoise2D(FVector2D((WorldLocation.X + Seed) / Scale, (WorldLocation.Y + Seed) / Scale)) * Amplitude;
//}

void UPerlinNoiseModifier::ApplyModifier(TArray<float>& HeightMap, int ChunkSize, FVector2D ChunkCoordinates) const
{
  if (HeightMap.IsEmpty()) return;

  for (int y = 0; y <= ChunkSize; y++)
  {
    for (int x = 0; x <= ChunkSize; x++)
    {
      int Index = x + y * (ChunkSize + 1);
      FVector2D WorldPos = ChunkCoordinates + FVector2D(x, y);
      float NoiseValue = FMath::PerlinNoise2D(WorldPos / Scale);

      if (ModifierType == ETerrainModifierType::Additive)
      {
        HeightMap[Index] += NoiseValue;
      }
      else if (ModifierType == ETerrainModifierType::Multiplicative)
      {
        HeightMap[Index] *= NoiseValue;
      }
    }
  }

  ApplyChildModifiers(HeightMap, ChunkSize, ChunkCoordinates);
}

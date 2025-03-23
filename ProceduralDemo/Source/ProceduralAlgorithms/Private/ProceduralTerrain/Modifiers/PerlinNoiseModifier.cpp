#include "ProceduralTerrain/Modifiers/PerlinNoiseModifier.h"

void UPerlinNoiseModifier::ApplyModifier(TArray<float>& HeightMap, int ChunkSize, FVector2D ChunkCoordinates) const
{
  if (HeightMap.IsEmpty()) return;

  for (int y = 0; y <= ChunkSize; y++)
  {
    for (int x = 0; x <= ChunkSize; x++)
    {
      int HeightMapIndex = x + y * (ChunkSize + 1);
      FVector2D WorldPos = ChunkCoordinates + FVector2D(x, y);
      float NoiseValue = FMath::PerlinNoise2D(WorldPos / Scale);

			if (Curve)
			{
				NoiseValue = Curve->GetFloatValue(NoiseValue);
			}

			NoiseValue *= Amplitude;


      switch (ModifierType)
      {
      case ETerrainModifierType::Additive:
        HeightMap[HeightMapIndex] += NoiseValue;
        break;

      case ETerrainModifierType::Multiplicative:
        HeightMap[HeightMapIndex] *= NoiseValue;
        break;

      default:
        UE_LOG(LogTemp, Warning, TEXT("TextureModifier - Unknown ModifierType."));
        break;
      }
    }
  }

  
}

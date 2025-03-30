#include "ProceduralTerrain/Modifiers/NoiseModifier.h"

void UNoiseModifier::ApplyModifier(TArray<float>& HeightMap, int ChunkSize, FVector2D ChunkCoordinates) const
{
	if (HeightMap.IsEmpty()) return;

	UFastNoiseWrapper* FastNoise = NewObject<UFastNoiseWrapper>();

	if (!FastNoise)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create FastNoise object."));
		return;
	}

	FastNoise->SetupFastNoise(NoiseType, Seed, Frequency, Interpolation, FractalType, Octaves, Lacunarity, Gain);

	for (int y = 0; y <= ChunkSize; y++)
	{
		for (int x = 0; x <= ChunkSize; x++)
		{
			int HeightMapIndex = x + y * (ChunkSize + 1);
			float NoiseValue = FastNoise->GetNoise2D(ChunkCoordinates.X + x, ChunkCoordinates.Y + y);


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

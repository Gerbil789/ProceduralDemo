#include "ProceduralTerrain/Modifiers/NoiseModifier.h"

void UNoiseModifier::ApplyModifier(float& Height, const FVector2D& Location) const
{
	UFastNoiseWrapper* FastNoise = NewObject<UFastNoiseWrapper>();

	if (!FastNoise)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create FastNoise object."));
		return;
	}

	FastNoise->SetupFastNoise(NoiseType, Seed, Frequency, Interpolation, FractalType, Octaves, Lacunarity, Gain, CellularJitter, CellularDistanceFunction, CellularReturnType);
	float NoiseValue = FastNoise->GetNoise2D(Location.X, Location.Y);

	switch (ModifierType)
	{
	case ETerrainModifierType::Additive:
		Height += NoiseValue;
		break;

	case ETerrainModifierType::Multiplicative:
		Height *= NoiseValue;
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("TextureModifier - Unknown ModifierType."));
		break;
	}
}

#include "ProceduralTerrain/Modifiers/NoiseModifier.h"

UNoiseModifier::UNoiseModifier()
{
	Initialize();
}

void UNoiseModifier::Initialize()
{
	FastNoise = NewObject<UFastNoiseWrapper>();

	if (!FastNoise)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create FastNoise object."));
	}
	FastNoise->SetupFastNoise(NoiseType, Seed, Frequency, Interpolation, FractalType, Octaves, Lacunarity, Gain, CellularJitter, CellularDistanceFunction, CellularReturnType);
}

void UNoiseModifier::ApplyModifier(float& Height, const FVector2D& Location) const
{
	if (!bEnabled) return;

	if (!FastNoise)
	{
		UE_LOG(LogTemp, Error, TEXT("FastNoise is null"));
		return;
	}

	float NoiseValue = (FastNoise->GetNoise2D(Location.X, Location.Y) + Add) * Scale;

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

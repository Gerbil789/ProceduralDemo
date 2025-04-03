#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralTerrain/Modifiers/TerrainModifier.h"
#include "FastNoiseWrapper.h"
#include "NoiseModifier.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class PROCEDURALALGORITHMS_API UNoiseModifier : public UTerrainModifier
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere)
	EFastNoise_NoiseType NoiseType = EFastNoise_NoiseType::Perlin;
	
	UPROPERTY(EditAnywhere)
	int32 Seed = 1337;

	UPROPERTY(EditAnywhere)
	float Frequency = 0.02f;

	UPROPERTY(EditAnywhere)
	EFastNoise_Interp Interpolation = EFastNoise_Interp::Quintic;
	
	UPROPERTY(EditAnywhere)
	EFastNoise_FractalType FractalType = EFastNoise_FractalType::FBM;

	UPROPERTY(EditAnywhere)
	int32 Octaves = 3;

	UPROPERTY(EditAnywhere)
	float Lacunarity = 2.0f;

	UPROPERTY(EditAnywhere)
	float Gain = 0.5f;

	UPROPERTY(EditAnywhere)
	float CellularJitter = 0.45f;

	UPROPERTY(EditAnywhere)
	EFastNoise_CellularDistanceFunction CellularDistanceFunction = EFastNoise_CellularDistanceFunction::Euclidean;

	UPROPERTY(EditAnywhere)
	EFastNoise_CellularReturnType CellularReturnType = EFastNoise_CellularReturnType::CellValue;

	UPROPERTY(EditAnywhere)
	ETerrainModifierType ModifierType = ETerrainModifierType::Additive;

	virtual void ApplyModifier(float& Height, const FVector2D& Location) const override;
};
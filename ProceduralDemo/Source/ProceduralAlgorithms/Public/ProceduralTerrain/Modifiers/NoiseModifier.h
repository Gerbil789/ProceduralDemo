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
	UNoiseModifier();

	virtual void Initialize() override;

	virtual void ApplyModifier(float& Height, const FVector2D& Location) const override;

	UPROPERTY(EditAnywhere, Category = "Modifier")
	EFastNoise_NoiseType NoiseType = EFastNoise_NoiseType::Perlin;
	
	UPROPERTY(EditAnywhere, Category = "Modifier", meta = (ToolTip = "Seed value used to generate the noise. Changing this gives different patterns."))
	int32 Seed = 1337;

	UPROPERTY(EditAnywhere, Category = "Modifier")
	float Frequency = 0.02f;

	UPROPERTY(EditAnywhere, Category = "Modifier", meta = (ToolTip = "Interpolation method used when sampling noise. Affects smoothness."))
	EFastNoise_Interp Interpolation = EFastNoise_Interp::Quintic;
	
	UPROPERTY(EditAnywhere, Category = "Modifier", meta = (ToolTip = "Fractal type used when generating layered noise"))
	EFastNoise_FractalType FractalType = EFastNoise_FractalType::FBM;

	UPROPERTY(EditAnywhere, Category = "Modifier", meta = (ToolTip = "Number of noise layers (octaves) used for fractal noise. More octaves = more detail."))
	int32 Octaves = 3;

	UPROPERTY(EditAnywhere, Category = "Modifier", meta = (ToolTip = "Multiplier for frequency between octaves in fractal noise."))
	float Lacunarity = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Modifier", meta = (ToolTip = "Multiplier for amplitude between octaves in fractal noise."))
	float Gain = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Modifier", meta = (ToolTip = "Jitter amount for cellular noise. Controls randomness of cell centers.", EditCondition = "NoiseType == EFastNoise_NoiseType::Cellular"))
	float CellularJitter = 0.45f;

	UPROPERTY(EditAnywhere, Category = "Modifier", meta = (ToolTip = "Function used to measure distance between cell centers in cellular noise.", EditCondition = "NoiseType == EFastNoise_NoiseType::Cellular"))
	EFastNoise_CellularDistanceFunction CellularDistanceFunction = EFastNoise_CellularDistanceFunction::Euclidean;

	UPROPERTY(EditAnywhere, Category = "Modifier", meta = (EditCondition = "NoiseType == EFastNoise_NoiseType::Cellular"))
	EFastNoise_CellularReturnType CellularReturnType = EFastNoise_CellularReturnType::CellValue;

	UPROPERTY(EditAnywhere, Category = "Modifier")
	ETerrainModifierType ModifierType = ETerrainModifierType::Additive;

	UPROPERTY(EditAnywhere, Category = "Modifier", meta = (ToolTip = "Offset added to the final noise output. Affects overall height contribution."))
	float Add = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Modifier", meta = (ToolTip = "Scale applied to the final noise output. Affects overall height contribution."))
	float Scale = 1.0f;

private:
	UPROPERTY() // Must be marked as UPROPERTY, otherwise it will be garbage collected at some point and break everything
	UFastNoiseWrapper* FastNoise = nullptr;
};
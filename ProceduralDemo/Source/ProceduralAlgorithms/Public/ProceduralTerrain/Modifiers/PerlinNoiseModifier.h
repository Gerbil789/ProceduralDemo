#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralTerrain/Modifiers/TerrainModifier.h"
#include "PerlinNoiseModifier.generated.h"


UCLASS(Blueprintable, EditInlineNew)
class PROCEDURALALGORITHMS_API UPerlinNoiseModifier : public UTerrainModifier
{
	GENERATED_BODY()
	
public:	
UPROPERTY(EditAnywhere, Category = "Perlin Noise")
float Scale = 100.0f;

//UPROPERTY(EditAnywhere, Category = "Perlin Noise")
//float Amplitude = 300.0f;

UPROPERTY(EditAnywhere, Category = "Perlin Noise")
int32 Seed = 0;

UPROPERTY(EditAnywhere, Category = "Perlin Noise")
ETerrainModifierType ModifierType = ETerrainModifierType::Additive;

//virtual float GetHeight_Implementation(FVector2D WorldLocation) const override;

virtual void ApplyModifier(TArray<float>& HeightMap, int ChunkSize, FVector2D ChunkCoordinates) const override;

};

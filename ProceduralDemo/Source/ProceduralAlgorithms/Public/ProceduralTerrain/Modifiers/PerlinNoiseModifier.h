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
	UPROPERTY(EditAnywhere)
	float Scale = 100.0f;
	
	UPROPERTY(EditAnywhere)
	UCurveFloat* Curve = nullptr;
	
	UPROPERTY(EditAnywhere)
	float Amplitude = 10000.0f;
	
	UPROPERTY(EditAnywhere)
	int32 Seed = 0;
	
	UPROPERTY(EditAnywhere)
	ETerrainModifierType ModifierType = ETerrainModifierType::Additive;
	
	virtual void ApplyModifier(TArray<float>& HeightMap, int ChunkSize, FVector2D ChunkCoordinates) const override;
};
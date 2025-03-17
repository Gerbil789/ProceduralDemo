#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralTerrain/Modifiers/TerrainModifier.h"
#include "ScaleModifier.generated.h"


UCLASS(Blueprintable, EditInlineNew)
class PROCEDURALALGORITHMS_API UScaleModifier : public UTerrainModifier
{
	GENERATED_BODY()

public:

	// Curve variable
	UPROPERTY(EditAnywhere, Category = "Scale")
	float Scale = 1.0f;

	virtual void ApplyModifier(TArray<float>& HeightMap, int ChunkSize, FVector2D ChunkCoordinates) const override;
};

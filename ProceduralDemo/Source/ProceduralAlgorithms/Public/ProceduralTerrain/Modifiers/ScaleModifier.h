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
	UPROPERTY(EditAnywhere, Category = "Modifier", meta = (ToolTip = "Multiply heightmap points by this value"))
	float Scale = 1.0f;

	virtual void ApplyModifier(float& Height, const FVector2D& Location) const override
	{
		if (!bEnabled) return;
		Height *= Scale;
	}
};

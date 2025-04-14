#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralTerrain/Modifiers/TerrainModifier.h"
#include "CurveModifier.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class PROCEDURALALGORITHMS_API UCurveModifier : public UTerrainModifier
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Modifier")
	UCurveFloat* Curve = nullptr;

	virtual void ApplyModifier(float& Height, const FVector2D& Location) const override
	{
		if (!bEnabled) return;
		if (!Curve) return;
		Height = Curve->GetFloatValue(Height);
	}
};

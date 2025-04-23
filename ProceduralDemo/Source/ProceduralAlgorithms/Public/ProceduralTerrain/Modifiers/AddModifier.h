#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralTerrain/Modifiers/TerrainModifier.h"
#include "AddModifier.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class PROCEDURALALGORITHMS_API UAddModifier : public UTerrainModifier
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Modifier", meta = (ToolTip = "Add this value to heightmap points"))
	float Add = 0.0f;

	virtual void ApplyModifier(float& Height, const FVector2D& Location) const override
	{
		if (!bEnabled) return;
		Height += Add;
	}
};

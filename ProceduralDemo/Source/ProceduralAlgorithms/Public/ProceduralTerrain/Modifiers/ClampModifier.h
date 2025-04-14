#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralTerrain/Modifiers/TerrainModifier.h"
#include "ClampModifier.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class PROCEDURALALGORITHMS_API UClampModifier : public UTerrainModifier
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Modifier")
	float MinHeight = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Modifier")
	float MaxHeight = 1000.0f;

	virtual void ApplyModifier(float& Height, const FVector2D& Location) const override
	{
		if (!bEnabled) return;
		Height = FMath::Clamp(Height, MinHeight, MaxHeight);
	}
};

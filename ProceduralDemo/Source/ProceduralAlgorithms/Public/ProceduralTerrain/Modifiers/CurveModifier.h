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
	UPROPERTY(EditAnywhere)
	UCurveFloat* Curve = nullptr;

	virtual void ApplyModifier(TArray<float>& HeightMap, int ChunkSize, FVector2D ChunkCoordinates) const override;
};

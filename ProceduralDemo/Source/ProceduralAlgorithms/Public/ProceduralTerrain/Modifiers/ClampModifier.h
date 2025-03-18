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
	UPROPERTY(EditAnywhere)
	float MinHeight = 0.0f;

	UPROPERTY(EditAnywhere)
	float MaxHeight = 1000.0f;

	virtual void ApplyModifier(TArray<float>& HeightMap, int ChunkSize, FVector2D ChunkCoordinates) const override;
};

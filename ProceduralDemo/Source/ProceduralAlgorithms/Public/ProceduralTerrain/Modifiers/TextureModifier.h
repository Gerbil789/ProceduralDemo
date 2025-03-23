#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralTerrain/Modifiers/TerrainModifier.h"
#include "Engine/Texture2D.h"
#include "TextureModifier.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class PROCEDURALALGORITHMS_API UTextureModifier : public UTerrainModifier
{
	GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UTexture2D* Texture;

  UPROPERTY(EditAnywhere)
  ETerrainModifierType ModifierType = ETerrainModifierType::Additive;

	virtual void ApplyModifier(TArray<float>& HeightMap, int ChunkSize, FVector2D ChunkCoordinates) const override;
};

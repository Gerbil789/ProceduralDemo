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
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
  UTexture2D* Texture;

  UPROPERTY(EditAnywhere, Category = "Modifier")
  ETerrainModifierType ModifierType = ETerrainModifierType::Additive;

	virtual void ApplyModifier(float& Height, const FVector2D& Location) const override;

private:
  FColor* ImageData;
};

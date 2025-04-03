#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerrainModifier.generated.h"

UENUM(BlueprintType)
enum class ETerrainModifierType : uint8
{
	Additive     UMETA(DisplayName = "Additive"),
	Multiplicative UMETA(DisplayName = "Multiplicative")
};

UCLASS(Abstract)
class PROCEDURALALGORITHMS_API UTerrainModifier : public UObject
{
	GENERATED_BODY()
	
public:	
	virtual void ApplyModifier(float& Height, const FVector2D& Location) const {}
};

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
	//UPROPERTY(EditAnywhere, Instanced, Category = "Modifier")
	//TArray<UTerrainModifier*> ChildModifiers; // Hierarchical modifiers

	virtual void ApplyModifier(TArray<float>& HeightMap, int ChunkSize, FVector2D ChunkCoordinates) const {}

//protected:
//	void ApplyChildModifiers(TArray<float>& HeightMap, int ChunkSize, FVector2D ChunkWorldPosition) const
//	{
//		for (const UTerrainModifier* Modifier : ChildModifiers)
//		{
//			if (!Modifier) continue;
//			Modifier->ApplyModifier(HeightMap, ChunkSize, ChunkWorldPosition);
//		}
//	}
};

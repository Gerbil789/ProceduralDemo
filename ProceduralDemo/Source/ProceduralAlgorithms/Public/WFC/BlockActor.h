#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFC/Block.h"
#include <Components/TextRenderComponent.h>
#include "BlockActor.generated.h"

UCLASS()
class PROCEDURALALGORITHMS_API ABlockActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABlockActor();
	void Initialize(const FBlockWFC Block, UStaticMesh* WireframeMesh = nullptr);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WireframeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TArray<UTextRenderComponent*> TextComponents;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTextRenderComponent* NameComponent;
};

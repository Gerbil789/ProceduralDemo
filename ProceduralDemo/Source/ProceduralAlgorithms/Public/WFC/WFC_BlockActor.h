#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFC_Block.h"
#include <Components/TextRenderComponent.h>
#include "WFC_BlockActor.generated.h"

UCLASS()
class PROCEDURALALGORITHMS_API AWFC_BlockActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AWFC_BlockActor();
	void Initialize(const FWFC_Block Block, UStaticMesh* WireframeMesh = nullptr);

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

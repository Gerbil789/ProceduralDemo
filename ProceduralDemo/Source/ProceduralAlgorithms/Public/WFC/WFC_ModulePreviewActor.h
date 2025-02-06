#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFC_Module.h"
#include <Components/TextRenderComponent.h>
#include "WFC_ModulePreviewActor.generated.h"

UCLASS()
class PROCEDURALALGORITHMS_API AWFC_ModulePreviewActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AWFC_ModulePreviewActor();
	void Initialize(const FWFC_Module Block, UStaticMesh* WireframeMesh = nullptr);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WireframeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TArray<UTextRenderComponent*> TextComponents;
};

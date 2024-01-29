// Fill out your copyright notice in the Description page of Project Settings.

#include "DebugSphereActor.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"

ADebugSphereActor::ADebugSphereActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ADebugSphereActor::SetValue(const float& val)
{
	this->value = val;

	//change material color
	UStaticMeshComponent* StaticMeshComponent = this->FindComponentByClass<UStaticMeshComponent>();
	if (!StaticMeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("StaticMeshComponent not found"));
		return;
	}

	//calculate color
	auto Color = FLinearColor::LerpUsingHSV(FLinearColor::Black, FLinearColor::White, this->value);

	// Get the material applied to the static mesh
	UMaterialInterface* OriginalMaterial = StaticMeshComponent->GetMaterial(0);

	// Create a dynamic material instance
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(OriginalMaterial, this);

	// Set the color parameter on the dynamic material
	DynamicMaterial->SetVectorParameterValue(TEXT("Color"), Color);

	// Apply the dynamic material to the static mesh component
	StaticMeshComponent->SetMaterial(0, DynamicMaterial);

	//set text
	UTextRenderComponent* TextRenderComponent = this->FindComponentByClass<UTextRenderComponent>();
	if (!TextRenderComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("TextRenderComponent not found in spawned actor."));
	}
	FString FormattedText = FString::Printf(TEXT("%.2f"), this->value);
	TextRenderComponent->SetText(FText::FromString(FormattedText));
}



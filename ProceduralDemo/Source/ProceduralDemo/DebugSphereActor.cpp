// Fill out your copyright notice in the Description page of Project Settings.

#include "DebugSphereActor.h"
#include "Components/SphereComponent.h"


ADebugSphereActor::ADebugSphereActor()
{
	PrimaryActorTick.bCanEverTick = true;

	
}

void ADebugSphereActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	FVector ActorLocation = this->GetActorLocation();
	TextRenderComponent = this->FindComponentByClass<UTextRenderComponent>();

	FString FormattedText = FString::Printf(TEXT("%.2f\n[%d,%d,%d]"), this->value, FMath::RoundToInt(ActorLocation.X), FMath::RoundToInt(ActorLocation.Y), FMath::RoundToInt(ActorLocation.Z));
	TextRenderComponent->SetText(FText::FromString(FormattedText));

}



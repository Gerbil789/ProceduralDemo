#pragma once

#include "CoreMinimal.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "DebugSphereActor.generated.h"

UCLASS()
class PROCEDURALDEMO_API ADebugSphereActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADebugSphereActor();
	virtual void Tick(float DeltaTime) override;

	void SetValue(const float& val);

	float value = -10.0f;
private:

	UTextRenderComponent* TextRenderComponent;
};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DebugSphereActor.generated.h"

UCLASS()
class PROCEDURALDEMO_API ADebugSphereActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADebugSphereActor();
	void SetValue(const float& val);

	float value = -10.0f;
};

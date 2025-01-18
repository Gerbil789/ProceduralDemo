#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestActor.generated.h"

// ***** IMPORTANT *****
// This actor is used for testing various features during development
// It is not intended to be used in the final game
// Its code is ugly and temporary, just for tesing purposes
// ***** IMPORTANT *****

UCLASS()
class PROCEDURALDEMO_API ATestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UFUNCTION(CallInEditor, Category = "Custom Tools")
	//void LoadMeshes();

	//UPROPERTY(EditAnywhere, Category = "Custom Tools")
	//FString MeshDirectoryPath = "/Game/TestMeshes";

};

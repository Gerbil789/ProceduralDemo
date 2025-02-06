#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "WFC_DataSet.h"
#include "WFC/WaveFunctionCollapse.h"
#include "Components/TextRenderComponent.h"
#include "TimerManager.h"
#include "WFC_DemoActor.generated.h"

UCLASS()
class PROCEDURALALGORITHMS_API AWFC_DemoActor : public AWaveFunctionCollapse
{
	GENERATED_BODY()
	
public:	
	AWFC_DemoActor();

protected:
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent); // Reload dataset when changed

	UFUNCTION(CallInEditor, Category = "WFC", meta = (DisplayName = "Initialize"))
	void Start();

	UFUNCTION(CallInEditor, Category = "WFC")
	void Next();

	UFUNCTION(CallInEditor, Category = "WFC")
	void Finish();

	UFUNCTION(CallInEditor, Category = "WFC", meta = (DisplayName = "Reload"))
	void LoadDataset();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "WFC", meta = (ToolTip = "Delete all mesh instances & Clean Grid"), meta = (DisplayName = "CleanUp"))
	void CleanUpMesh();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	UWFC_DataSet* Dataset = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	float DelayDuration = 2.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	int Offset = 200;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	bool Debug = false;



private:
	FTimerHandle IterationTimerHandle;
	bool Iterate();
	void IterateWithDelay();
	bool bIsIterating = false;

	TMap<UStaticMesh*, UInstancedStaticMeshComponent*> ISMComponents = TMap<UStaticMesh*, UInstancedStaticMeshComponent*>();

	TMap<FIntVector, class UTextRenderComponent*> TextComponents = TMap<FIntVector, class UTextRenderComponent*>();
	
};

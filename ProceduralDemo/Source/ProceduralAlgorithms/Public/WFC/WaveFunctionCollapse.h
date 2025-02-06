#pragma once

#include "CoreMinimal.h"
#include "WFC_Module.h"
#include "WFC_ModuleSet.h"
#include "WaveFunctionCollapse.generated.h" 

UCLASS(Blueprintable, Abstract)
class PROCEDURALALGORITHMS_API AWaveFunctionCollapse : public AActor
{
	GENERATED_BODY()

public:
	AWaveFunctionCollapse() = default;

	UFUNCTION(BlueprintCallable, Category = "WFC")
	void SetModules(const TArray<FWFC_Module>& _Modules);

	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool Run();

	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool Initialize();

	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool FindLowestEntropySlot(FIntVector& OutPosition);

	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool CollapseSlot(const FIntVector& Position);

	UFUNCTION(BlueprintCallable, Category = "WFC")
	bool CollapseSlotToModule(const FIntVector& Position, const FWFC_Module& Module);

	UFUNCTION(BlueprintCallable, Category = "WFC")
	void Propagate(const FIntVector& Position);

protected:
	int GetIndex(const FIntVector& Position) const;

private:
	void BuildCompatibilityTable();
	static const TArray<FIntVector> Directions;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "WFC")
	TArray<FWFC_Module> Modules;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	FIntVector GridSize = FIntVector(10, 10, 1);

	UPROPERTY(BlueprintReadWrite, Category = "WFC")
	TMap<FIntVector, FWFC_Module> Grid;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	bool RandomSeed = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	int Seed = 7;

	TArray<WFC_ModuleSet> Wave;

private:
	FRandomStream RandomStream;
	TMap<FIntVector, TMap<FWFC_Module, TArray<FWFC_Module>>> CompatibilityTable;
};
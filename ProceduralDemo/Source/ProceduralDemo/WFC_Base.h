#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFC_Base.generated.h"

UCLASS()
class PROCEDURALDEMO_API AWFC_Base : public AActor
{
	GENERATED_BODY()
	
public:	
	AWFC_Base();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC_Block", Meta = (DisplayName = "Top    (+Z)"))
    FString Top;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC_Block", Meta = (DisplayName = "Bottom (-Z)"))
    FString Bottom;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC_Block", Meta = (DisplayName = "Left   (-X)"))
    FString Left;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC_Block", Meta = (DisplayName = "Right  (+X)"))
    FString Right;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC_Block", Meta = (DisplayName = "Front  (+Y)"))
    FString Front;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC_Block", Meta = (DisplayName = "Back   (-Y)"))
    FString Back;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC_Block")
    int Priority;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC_Block")
    int Variants;



};

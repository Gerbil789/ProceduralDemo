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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC_Block")
	UStaticMesh* Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block_Sockets", Meta = (DisplayName = "Top    (+Z)"))
    FString Top = "TOP";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block_Sockets", Meta = (DisplayName = "Bottom (-Z)"))
    FString Bottom = "BOTTOM";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block_Sockets", Meta = (DisplayName = "Left   (-X)"))
    FString Left = "LEFT";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block_Sockets", Meta = (DisplayName = "Right  (+X)"))
    FString Right = "RIGHT";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block_Sockets", Meta = (DisplayName = "Front  (+Y)"))
    FString Front = "FRONT";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block_Sockets", Meta = (DisplayName = "Back   (-Y)"))
    FString Back = "BACK";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other")
    int Priority = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Other")
    int Variants = 1;
};

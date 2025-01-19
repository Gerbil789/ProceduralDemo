#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFC_Block.h"
#include "WFC_BlockActor.h"
#include "WFC_BlockPreviewActor.generated.h"

// This class is used to preview blocks in the editor, so i can inspect if the sockets are placed correctly
// Its not used in the actual WFC algorithm
// Its pure C++ class, no blueprint
UCLASS()
class PROCEDURALALGORITHMS_API AWFC_BlockPreviewActor : public AActor
{
	GENERATED_BODY()
	
public:	
  AWFC_BlockPreviewActor();

  UPROPERTY(EditAnywhere, Category = "WFC")
  FString MeshDirectoryPath = "/Game/TestMeshes";

  UPROPERTY(EditAnywhere, Category = "WFC")
	UStaticMesh* WireframeMesh = nullptr;

  UPROPERTY(EditAnywhere, Category = "WFC")
  bool bAddEmptyBlock = true;

  UPROPERTY(EditAnywhere, Category = "WFC")
  bool bAddFillBlock = true;

  UPROPERTY(EditAnywhere, Category = "WFC")
  int Offset = 400;

  UFUNCTION(CallInEditor, Category = "WFC")
  void LoadMeshes();

  UFUNCTION(CallInEditor, Category = "WFC")
  void Preview();

  UFUNCTION(CallInEditor, Category = "WFC")
  void Clear();

  UFUNCTION(CallInEditor, Category = "WFC")
  void Save();

  UPROPERTY(EditAnywhere, Category = "WFC")
  FString SaveAssetPath = "/Game/Save/Dataset";

private:
  TArray<FWFC_Block> Blocks;
};
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFC_Module.h"
#include "WFC_ModulePreviewActor.h"
#include "WFC_DataProcessorActor.generated.h"

// This class is used to preview blocks in the editor, so i can inspect if the sockets are placed correctly
// Its not used in the actual WFC algorithm
// Its pure C++ class, no blueprint
UCLASS()
class PROCEDURALALGORITHMS_API AWFC_DataProcessorActor : public AActor
{
	GENERATED_BODY()
	
public:	
  AWFC_DataProcessorActor();

  UPROPERTY(EditAnywhere, Category = "WFC")
  FString SaveAssetPath = "/Game/Save";

  UPROPERTY(EditAnywhere, Category = "WFC")
  FString SaveAssetName = "newAsset";

  //-------------------------------------------------------------------------------------

  UPROPERTY(EditAnywhere, Category = "WFC_Dataset")
  FString MeshDirectoryPath = "/Game/WFC/Meshes";

  UPROPERTY(EditAnywhere, Category = "WFC_Dataset")
	UStaticMesh* WireframeMesh = nullptr;

  UPROPERTY(EditAnywhere, Category = "WFC_Dataset")
  bool bAddEmptyBlock = true;

  UPROPERTY(EditAnywhere, Category = "WFC_Dataset")
  bool bAddFillBlock = true;

  UPROPERTY(EditAnywhere, Category = "WFC_Dataset")
  int Offset = 400;

  UFUNCTION(CallInEditor, Category = "WFC_Dataset")
  void LoadMeshes();

  UFUNCTION(CallInEditor, Category = "WFC_Dataset")
  void Preview();

  UFUNCTION(CallInEditor, Category = "WFC_Dataset")
  void Clear();

  UFUNCTION(CallInEditor, Category = "WFC_Dataset")
  void SaveBlocks();

	//-------------------------------------------------------------------------------------

  UFUNCTION(CallInEditor, Category = "WFC_Block")
  void SaveBlock();

  UPROPERTY(EditAnywhere, Category = "WFC_Block")
  UStaticMesh* StaticMesh = nullptr;

private:
  TArray<FWFC_Module> Blocks;
};
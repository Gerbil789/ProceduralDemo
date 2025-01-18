#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFC/Block.h"
#include "WFC/BlockActor.h"
#include "BlockPreviewActor.generated.h"

// This class is used to preview blocks in the editor, so i can inspect if the sockets are placed correctly
// Its not used in the actual WFC algorithm
// Its pure C++ class, no blueprint
UCLASS()
class PROCEDURALALGORITHMS_API ABlockPreviewActor : public AActor
{
	GENERATED_BODY()
	
public:	
  ABlockPreviewActor();

  UPROPERTY(EditAnywhere, Category = "WFC")
  FString MeshDirectoryPath = "/Game/TestMeshes";

  UPROPERTY(EditAnywhere, Category = "WFC")
  UStaticMesh* WireframeMesh;

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

private:
  TArray<FBlockWFC> Blocks;
};
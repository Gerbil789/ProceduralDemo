#pragma once

#include "CoreMinimal.h"
#include "Socket.h"
#include "Block.h"

namespace WFC
{
  class PROCEDURALALGORITHMS_API Utility
  {
  public:
    //TODO: update this comment when its done...
    /**
    * Loads and processes all assets (specifically static meshes) from a specified directory within the Unreal project.
    *
    * This function checks if the provided directory path exists and contains assets. If the directory is valid,
    * it retrieves all assets in the directory, processes each asset, and displays a progress dialog to the user.
    * If the directory is invalid, empty, or contains no assets, an error dialog is displayed.
    *
    * @param DirectoryPath - The virtual path to the directory (e.g., "/Game/TestMeshes").
    *                        Must be relative to the "/Game/" directory in Unreal.
    */
    static bool LoadBlocksFromDirectory(const FString& DirectoryPath, TArray<FBlockWFC>& OutBlocks, bool AddEmpty = false, bool AddFill = false);

  private:
		static bool CreateBlocks(const FString& AssetName, UStaticMesh* Mesh, TArray<FBlockWFC>& OutBlocks);
		static FBlockWFC RotateBlock(const FBlockWFC& Block, const int& Rotation);
  };

}

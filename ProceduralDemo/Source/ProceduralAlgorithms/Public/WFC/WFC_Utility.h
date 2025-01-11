#pragma once

#include "CoreMinimal.h"

namespace WFC
{
  class PROCEDURALALGORITHMS_API WFC_Utility
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
    static void LoadMeshesFromFolder(const FString& DirectoryPath);
  };

}

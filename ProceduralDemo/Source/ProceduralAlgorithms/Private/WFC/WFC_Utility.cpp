#include "WFC/WFC_Utility.h"
#include "Utility.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/StaticMesh.h"
#include "Misc/Paths.h"

namespace WFC
{
  void WFC_Utility::LoadMeshesFromFolder(const FString& DirectoryPath)
  {
    // Check if the directory path is empty
    if (DirectoryPath.IsEmpty())
    {
      Utility::ShowMessage("Directory path is empty.", __FUNCTION__);
      return;
    }

    // Check if the directory exists
    FString FullDirectoryPath = FPaths::ProjectContentDir() + DirectoryPath.RightChop(6); // Strip the "/Game/" prefix
    if (!FPaths::DirectoryExists(FullDirectoryPath))
    {
      Utility::ShowMessage(FString::Printf(TEXT("Directory does not exist: %s"), *FullDirectoryPath), __FUNCTION__);
      return;
    }

    // Load the AssetRegistry module
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    AssetRegistryModule.Get().SearchAllAssets(true);

    // Retrieve assets from the directory
    TArray<FAssetData> AssetDataArray;
    AssetRegistryModule.Get().GetAssetsByPath(FName(*DirectoryPath), AssetDataArray, true);

    // Check if the directory contains any assets
    if (AssetDataArray.Num() == 0)
    {
      Utility::ShowMessage(FString::Printf(TEXT("No assets found in directory: %s"), *DirectoryPath), __FUNCTION__);
      return;
    }

    // Create a progress dialog
    FScopedSlowTask ProgressTask(AssetDataArray.Num(), FText::FromString(TEXT("Processing meshes...")));
    ProgressTask.MakeDialog(); // Show progress dialog

    int StaticMeshCount = 0;
    for (const FAssetData& AssetData : AssetDataArray)
    {
      //UObject* Asset = AssetData.GetAsset();
      UStaticMesh* StaticMesh = Cast<UStaticMesh>(AssetData.GetAsset());
      if (!StaticMesh)
      {
        continue;
      }

      FString AssetName = AssetData.AssetName.ToString();

			UE_LOG(LogTemp, Display, TEXT("Processing static mesh: %s"), *AssetName);
  
      StaticMeshCount++;

      ProgressTask.EnterProgressFrame(1.0f); // Update progress dialog
    }

    UE_LOG(LogTemp, Display, TEXT("---------------------------------"));
    UE_LOG(LogTemp, Warning, TEXT("Processed %d static meshes.\n"), StaticMeshCount);
  }
}

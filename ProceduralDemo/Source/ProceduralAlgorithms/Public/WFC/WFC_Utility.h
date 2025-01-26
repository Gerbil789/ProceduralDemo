#pragma once

#include "CoreMinimal.h"
#include "WFC_Socket.h"
#include "WFC_Block.h"

class PROCEDURALALGORITHMS_API WFC_Utility
{
public:
	/**
	* Loads WFC blocks from a specified directory in the project.
	*
	* @param DirectoryPath The relative path to the directory (e.g., "/Game/MyAssets").
	* @param OutBlocks An array to store the loaded WFC blocks.
	* @param AddEmpty If true, an "empty" block is added to the result.
	* @param AddFill If true, a "fill" block is added to the result.
	* @return True if the blocks were successfully loaded, false otherwise.
	*
	* This function validates the directory path, loads static mesh assets from it,
	* and processes them into WFC blocks. Optionally, it can add predefined "empty"
	* and "fill" blocks to the result. Progress is displayed in the editor via a
	* progress dialog. Logs and dialog messages provide feedback on the operation.
	*/
	static bool LoadBlocksFromDirectory(const FString& DirectoryPath, TArray<FWFC_Block>& OutBlocks, bool AddEmpty = false, bool AddFill = false);


	/**
	* Saves WFC block data to a specified asset path in the project.
	*
	* @param AssetPath The full asset path (e.g., "/Game/MyAssets/MyDataAsset").
	*                  The path must start with "/Game/" to be valid.
	* @param Blocks An array of WFC blocks to be saved into the asset.
	* @return True if the data asset was successfully created and saved, false otherwise.
	*
	* This function creates a new package and a data asset at the specified asset path.
	* It ensures the directory exists, registers the new asset with the Asset Registry,
	* and saves the package to disk. Logs provide detailed feedback on the process,
	* including errors for invalid paths or save failures.
	*/
	static bool SaveData(const FString& AssetPath, const TArray<FWFC_Block>& Blocks);

	static bool SaveData(const FString& AssetPath, const FWFC_Block& Block);

	static bool CreateSockets(TArray<FString> Tokens, TArray<FWFC_Socket>& OutSockets);

private:
	static bool CreateBlocks(const FString& AssetName, UStaticMesh* Mesh, TArray<FWFC_Block>& OutBlocks);
	static FWFC_Block RotateBlock(const FWFC_Block& Block, const int& Rotation);
};



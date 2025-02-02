#include "WFC/WFC_Utility.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/StaticMesh.h"
#include "WFC/WFC_DataSet.h"
#include "Misc/Paths.h"

bool WFC_Utility::LoadBlocksFromDirectory(const FString& DirectoryPath, TArray<FWFC_Module>& OutBlocks, bool AddEmpty, bool AddFill)
{
	OutBlocks.Empty();

	// Check if the directory path is empty
	if (DirectoryPath.IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Directory path is empty."), FText::FromString(__FUNCTION__));
		return false;
	}

	// Check if the directory exists
	FString FullDirectoryPath = FPaths::ProjectContentDir() + DirectoryPath.RightChop(6); // Strip the "/Game/" prefix
	if (!FPaths::DirectoryExists(FullDirectoryPath))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(FString::Printf(TEXT("Directory does not exist: %s"), *FullDirectoryPath)), FText::FromString(__FUNCTION__));
		return false;
	}

	// Load the AssetRegistry module (to access directories inside project)
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetRegistryModule.Get().SearchAllAssets(true);

	// Retrieve assets from the directory
	TArray<FAssetData> AssetDataArray;
	AssetRegistryModule.Get().GetAssetsByPath(FName(*DirectoryPath), AssetDataArray, true);

	// Check if the directory contains any assets
	if (AssetDataArray.Num() == 0)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(FString::Printf(TEXT("No assets found in directory: %s"), *DirectoryPath)), FText::FromString(__FUNCTION__));
		return false;
	}

	// Create a progress dialog
#if WITH_EDITOR
	FScopedSlowTask ProgressTask(AssetDataArray.Num(), FText::FromString(TEXT("Processing meshes...")));
	ProgressTask.MakeDialog(); // Show progress dialog
#endif

	int StaticMeshCount = 0;
	int SuccessCount = 0;

	// Process each asset in the directory
	for (const FAssetData& AssetData : AssetDataArray)
	{
#if WITH_EDITOR
		ProgressTask.EnterProgressFrame(1.0f); // Update progress dialog
#endif
		UStaticMesh* StaticMesh = Cast<UStaticMesh>(AssetData.GetAsset());
		if (!StaticMesh)
		{
			continue; // this will exclude materials and other assets that might be in the directory
		}

		StaticMeshCount++;
		FString AssetName = AssetData.AssetName.ToString();

		TArray<FWFC_Module> NewBlocks;
		if (CreateBlocks(AssetName, StaticMesh, NewBlocks) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create block from asset name: %s"), *AssetName);
			continue;
		}

		OutBlocks.Append(NewBlocks);
		SuccessCount++;
	}

	UE_LOG(LogTemp, Display, TEXT("---------------------------------"));
	UE_LOG(LogTemp, Warning, TEXT("Processed %d/%d Assets."), SuccessCount, StaticMeshCount);
	UE_LOG(LogTemp, Warning, TEXT("Created %d WFC Blocks."), OutBlocks.Num());

	// Display a message dialog if not all assets were processed
	if (SuccessCount == 0 || SuccessCount != StaticMeshCount)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(FString::Printf(TEXT("Processed %d/%d WFC blocks.\n"), SuccessCount, StaticMeshCount)), FText::FromString(__FUNCTION__));
	}

	// Add empty and fill blocks if requested
	if (AddEmpty)
	{
		FWFC_Socket EmptySocketHorizontal = FWFC_Socket(0, true, true, false, false);
		FWFC_Socket EmptySocketVertical = FWFC_Socket(0, false, false, false, true);
		TArray<FWFC_Socket> EmptySockets = { EmptySocketHorizontal, EmptySocketHorizontal, EmptySocketHorizontal, EmptySocketHorizontal, EmptySocketVertical, EmptySocketVertical };
		FWFC_Module EmptyBlock = FWFC_Module(nullptr, EmptySockets);
		OutBlocks.Add(EmptyBlock);
	}

	if (AddFill)
	{
		FWFC_Socket FillSocketHorizontal = FWFC_Socket(1, true, true, false, false);
		FWFC_Socket FillSocketVertical = FWFC_Socket(1, false, false, false, true);
		TArray<FWFC_Socket> FillSockets = { FillSocketHorizontal, FillSocketHorizontal, FillSocketHorizontal, FillSocketHorizontal, FillSocketVertical, FillSocketVertical };
		FWFC_Module FillBlock = FWFC_Module(nullptr, FillSockets);
		OutBlocks.Add(FillBlock);
	}

	return true;
}

bool WFC_Utility::SaveData(const FString& AssetPath, const TArray<FWFC_Module>& Modules)
{
	if (!AssetPath.StartsWith(TEXT("/Game/")))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid asset path: %s. Path must start with '/Game/'."), *AssetPath);
		return false;
	}

	FString PackageName = FPackageName::ObjectPathToPackageName(AssetPath);
	FString AssetName = FPackageName::GetLongPackageAssetName(PackageName);

	// Ensure the folder exists
	FString DirectoryPath = FPaths::ProjectContentDir() + FPackageName::GetLongPackagePath(PackageName).Mid(6); // Remove "/Game/"
	if (!IFileManager::Get().DirectoryExists(*DirectoryPath))
	{
		if (!IFileManager::Get().MakeDirectory(*DirectoryPath, true))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create directory: %s"), *DirectoryPath);
			return false;
		}
	}

	//check if package already exists
	if (FPackageName::DoesPackageExist(PackageName))
	{
		if (FMessageDialog::Open(EAppMsgType::OkCancel, FText::FromString(FString::Printf(TEXT("Package already exists: %s. Overwrite?"), *PackageName)), FText::FromString(__FUNCTION__)) == EAppReturnType::Cancel)
		{
			return false;
		}
	}

	// Create a new package
	UPackage* Package = CreatePackage(*PackageName);
	if (!Package)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create package: %s"), *PackageName);
		return false;
	}

	// Create the data asset instance
	UWFC_DataSet* DataAsset = NewObject<UWFC_DataSet>(Package, UWFC_DataSet::StaticClass(), *AssetName, RF_Public | RF_Standalone);
	if (!DataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create data asset."));
		return false;
	}

	DataAsset->Modules = Modules;

	// Mark the package dirty and save it
	FAssetRegistryModule::AssetCreated(DataAsset);
	DataAsset->MarkPackageDirty();

	FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());

	if (!UPackage::SavePackage(Package, DataAsset, RF_Public | RF_Standalone, *PackageFileName))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save package: %s"), *PackageFileName);
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Data asset '%s' saved at '%s'"), *PackageName, *PackageFileName);
	return true;

}

bool WFC_Utility::SaveData(const FString& AssetPath, const FWFC_Module& Module)
{
	if (!AssetPath.StartsWith(TEXT("/Game/")))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid asset path: %s. Path must start with '/Game/'."), *AssetPath);
		return false;
	}


	FString PackageName = FPackageName::ObjectPathToPackageName(AssetPath);
	FString AssetName = FPackageName::GetLongPackageAssetName(PackageName);

	// Ensure the folder exists
	FString DirectoryPath = FPaths::ProjectContentDir() + FPackageName::GetLongPackagePath(PackageName).Mid(6); // Remove "/Game/"
	if (!IFileManager::Get().DirectoryExists(*DirectoryPath))
	{
		if (!IFileManager::Get().MakeDirectory(*DirectoryPath, true))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create directory: %s"), *DirectoryPath);
			return false;
		}
	}

	FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());

	//check if package already exists
	if (FPackageName::DoesPackageExist(PackageName))
	{
		if (FMessageDialog::Open(EAppMsgType::OkCancel, FText::FromString(FString::Printf(TEXT("Package already exists: %s. Overwrite?"), *PackageName)), FText::FromString(__FUNCTION__)) == EAppReturnType::Cancel)
		{
			return false;
		}

		// delete the existing package
		if (!IFileManager::Get().Delete(*PackageFileName))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to delete existing package: %s"), *PackageFileName);
			return false;
		}
	}

	// Create a new package
	UPackage* Package = CreatePackage(*PackageName);
	if (!Package)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create package: %s"), *PackageName);
		return false;
	}

	// Create the data asset instance
	UWFC_ModuleAsset* DataAsset = NewObject<UWFC_ModuleAsset>(Package, UWFC_ModuleAsset::StaticClass(), *AssetName, RF_Public | RF_Standalone);
	if (!DataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create data asset."));
		return false;
	}

	DataAsset->Module = Module;

	// Mark the package dirty and save it
	FAssetRegistryModule::AssetCreated(DataAsset);
	DataAsset->MarkPackageDirty();


	if (!UPackage::SavePackage(Package, DataAsset, RF_Public | RF_Standalone, *PackageFileName))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save package: %s"), *PackageFileName);
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Data asset '%s' saved at '%s'"), *PackageName, *PackageFileName);
	return true;
}


bool WFC_Utility::CreateSockets(TArray<FString> Tokens, TArray<FWFC_Socket>& OutSockets)
{
	if (Tokens.Num() < 6)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid number of socket strings in asset name"));
		return false;
	}

	OutSockets.Empty();
	OutSockets.Reserve(6);

	// Process each socket string
	for (int i = 0; i < 6; i++)
	{
		if (Tokens[i].IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("Socket string is empty."));
			return false;
		}

		FWFC_Socket Socket;

		// First 4 are horizonal, last 2 are vertical
		// Front, Back, Right, Left, Up, Down
		if (i < 4)
		{
			Socket.bIsHorizontal = true;
		}
		else
		{
			Socket.bIsHorizontal = false;
		}

		// Check if the socket is symmetric
		if (Tokens[i].EndsWith(TEXT("s")))
		{
			Socket.bIsSymmetric = true;
			Tokens[i].RemoveFromEnd(TEXT("s"));
		}

		// Check if the socket is flipped
		if (Tokens[i].EndsWith(TEXT("f")))
		{
			Socket.bIsFlipped = true;
			Tokens[i].RemoveFromEnd(TEXT("f"));
		}

		// Check if the socket is irrelevant to rotation
		if (Tokens[i].EndsWith(TEXT("i")))
		{
			Socket.bIsInvariantRotation = true;
			Tokens[i].RemoveFromEnd(TEXT("i"));
		}

		Socket.Id = FCString::Atoi(*Tokens[i]);
		OutSockets.Add(Socket);
	}

	return true;
}

bool WFC_Utility::CreateBlocks(const FString& AssetName, UStaticMesh* Mesh, TArray<FWFC_Module>& OutBlocks)
{
	if (AssetName.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Asset name is empty."));
		return false;
	}

	// Split the asset name into an array of strings
	TArray<FString> Tokens;
	AssetName.ParseIntoArray(Tokens, TEXT("_"), true);

	// Check if the asset name contains the correct number of socket strings
	if (Tokens.Num() < 6)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid number of socket strings in asset name: %s"), *AssetName);
		return false;
	}

	TArray<FWFC_Socket> Sockets;
	if(!CreateSockets(Tokens, Sockets))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create sockets for mesh: %s"), *AssetName);
		return false;;
	}

	// Create the first block
	FWFC_Module NewBlock = FWFC_Module(Mesh, Sockets);
	OutBlocks.Add(NewBlock);

	// Add additional blocks if there are variants
	int Variants = 1;
	if (Tokens.Num() == 7)
	{
		Variants = FCString::Atoi(*Tokens[6]);
	}

	for (int i = 1; i < Variants; i++)
	{
		FWFC_Module RotatedBlock = RotateBlock(NewBlock, (i * 90));
		OutBlocks.Add(RotatedBlock);

	}

	return true;
}


FWFC_Module WFC_Utility::RotateBlock(const FWFC_Module& Block, const int& Rotation)
{
	FWFC_Module RotatedBlock = Block;

	if (Rotation % 90 != 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid rotation value: %d"), Rotation);
		return RotatedBlock;
	}

	RotatedBlock.Rotation = Rotation;

	// Rotate the sockets
	if (Rotation == 90)
	{
		RotatedBlock.SocketFront = Block.SocketLeft;
		RotatedBlock.SocketBack = Block.SocketRight;
		RotatedBlock.SocketRight = Block.SocketFront;
		RotatedBlock.SocketLeft = Block.SocketBack;
	}
	else if (Rotation == 180)
	{
		RotatedBlock.SocketFront = Block.SocketBack;
		RotatedBlock.SocketBack = Block.SocketFront;
		RotatedBlock.SocketRight = Block.SocketLeft;
		RotatedBlock.SocketLeft = Block.SocketRight;
	}
	else if (Rotation == 270)
	{
		RotatedBlock.SocketFront = Block.SocketRight;
		RotatedBlock.SocketBack = Block.SocketLeft;
		RotatedBlock.SocketRight = Block.SocketBack;
		RotatedBlock.SocketLeft = Block.SocketFront;
	}

	if (!RotatedBlock.SocketUp.bIsInvariantRotation)
	{
		RotatedBlock.SocketUp.Rotation = Rotation;
	}

	if (!RotatedBlock.SocketDown.bIsInvariantRotation)
	{
		RotatedBlock.SocketDown.Rotation = Rotation;
	}

	return RotatedBlock;
}

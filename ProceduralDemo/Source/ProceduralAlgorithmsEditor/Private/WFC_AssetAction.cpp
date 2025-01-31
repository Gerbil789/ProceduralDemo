#include "WFC_AssetAction.h"
#include "WFC/WFC_Utility.h"

bool UWFC_AssetAction::ProcessStaticMeshes(const TArray<UStaticMesh*>& StaticMeshes, bool bAddEmpty, bool bAddFill, TArray<FWFC_Block>& OutBlocks)
{
	OutBlocks.Empty();
	int SuccessCount = 0;
	for (UStaticMesh* StaticMesh : StaticMeshes)
	{
		FString AssetName = StaticMesh->GetName();
		
		TArray<FWFC_Block> NewBlocks;
		if (!WFC_Utility::CreateBlocks(AssetName, StaticMesh, NewBlocks))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create block from asset name: %s"), *AssetName);
			continue;
		}
		OutBlocks.Append(NewBlocks);
		SuccessCount++;
	}

	// Add empty and fill blocks if requested
	if (bAddEmpty)
	{
		FWFC_Socket EmptySocketHorizontal = FWFC_Socket(0, true, true, false, false);
		FWFC_Socket EmptySocketVertical = FWFC_Socket(0, false, false, false, true);
		TArray<FWFC_Socket> EmptySockets = { EmptySocketHorizontal, EmptySocketHorizontal, EmptySocketHorizontal, EmptySocketHorizontal, EmptySocketVertical, EmptySocketVertical };
		FWFC_Block EmptyBlock = FWFC_Block(nullptr, EmptySockets);
		OutBlocks.Add(EmptyBlock);
	}

	if (bAddFill)
	{
		FWFC_Socket FillSocketHorizontal = FWFC_Socket(1, true, true, false, false);
		FWFC_Socket FillSocketVertical = FWFC_Socket(1, false, false, false, true);
		TArray<FWFC_Socket> FillSockets = { FillSocketHorizontal, FillSocketHorizontal, FillSocketHorizontal, FillSocketHorizontal, FillSocketVertical, FillSocketVertical };
		FWFC_Block FillBlock = FWFC_Block(nullptr, FillSockets);
		OutBlocks.Add(FillBlock);
	}

	UE_LOG(LogTemp, Display, TEXT("---------------------------------"));
	UE_LOG(LogTemp, Warning, TEXT("Processed %d/%d Assets."), SuccessCount, StaticMeshes.Num());
	UE_LOG(LogTemp, Warning, TEXT("Created %d WFC Blocks."), OutBlocks.Num());

	return true;
}


bool UWFC_AssetAction::SaveDataSet(const FString& AssetPath, const TArray<FWFC_Block>& Blocks)
{
	if (Blocks.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("No blocks to save."));
		return false;
	}

	if (!WFC_Utility::SaveData(AssetPath, Blocks))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save blocks to: %s"), *AssetPath);
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Saved blocks to: %s"), *AssetPath);
	return true;
}

bool UWFC_AssetAction::ProcessStaticMesh(UStaticMesh* StaticMesh, FWFC_Block& OutBlock)
{
	if (!StaticMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("StaticMesh is not set."));
		return false;
	}

	FString AssetName = StaticMesh->GetName();

	TArray<FString> Tokens;
	AssetName.ParseIntoArray(Tokens, TEXT("_"), true);

	if (Tokens.Num() < 6)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid number of socket strings in asset name: %s"), *AssetName);
		return false;
	}

	TArray<FWFC_Socket> Sockets;
	if (!WFC_Utility::CreateSockets(Tokens, Sockets))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create sockets for mesh: %s"), *AssetName);
		return false;
	}

	OutBlock = FWFC_Block(StaticMesh, Sockets);
	return true;
}

bool UWFC_AssetAction::SaveBlock(const FString& AssetPath, const FWFC_Block& Block)
{
	if (!WFC_Utility::SaveData(AssetPath, Block))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save block to: %s"), *AssetPath);
		return false;
	}
	UE_LOG(LogTemp, Log, TEXT("Saved block to: %s"), *AssetPath);
	return true;
}

void UWFC_AssetAction::RecalculatePLogP(UWFC_DataSet* DataSet)
{
	//for (FWFC_Block& Block : DataSet->Blocks)
	//{
	//	Block.RecalculatePLogP();
	//}
}

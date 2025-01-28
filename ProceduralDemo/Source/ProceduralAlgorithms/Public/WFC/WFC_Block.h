#pragma once

#include "WFC_Socket.h"
#include "CoreMinimal.h"
#include "WFC_Block.generated.h"

USTRUCT(BlueprintType)
struct PROCEDURALALGORITHMS_API FWFC_Block
{
	GENERATED_BODY()

public:
	FWFC_Block() = default;

	FWFC_Block(UStaticMesh* StaticMesh, const TArray<FWFC_Socket>& Sockets, int Rotation = 0, int Priority = 1) :
		StaticMesh(StaticMesh),
		Rotation(Rotation),
		Priority(Priority),
		SocketFront(Sockets[0]),
		SocketBack(Sockets[1]),
		SocketRight(Sockets[2]),
		SocketLeft(Sockets[3]),
		SocketUp(Sockets[4]),
		SocketDown(Sockets[5])
	{
		//DisplayName = this->ToString();
	}

	//UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "WFC")
	//FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Block")
	UStaticMesh* StaticMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
	int Rotation = 0; // 0, 90, 180, 270

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
	int Priority = 1;

	// Sockets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
	FWFC_Socket SocketFront;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
	FWFC_Socket SocketBack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
	FWFC_Socket SocketRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
	FWFC_Socket SocketLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
	FWFC_Socket SocketUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
	FWFC_Socket SocketDown;

	bool IsEmpty() const { return StaticMesh == nullptr; }

	FString ToString() const
	{
		return FString::Printf(
			TEXT("%s_%s_%s_%s_%s_%s"),
			*SocketFront.ToString(),
			*SocketBack.ToString(),
			*SocketRight.ToString(),
			*SocketLeft.ToString(),
			*SocketUp.ToString(),
			*SocketDown.ToString()
		);
	}

	// Equality operator is required for TMap to work
	bool operator==(const FWFC_Block& Other) const
	{
		return StaticMesh == Other.StaticMesh && Rotation == Other.Rotation;
	}
};

// Hash function is required for TMap to work
inline uint32 GetTypeHash(const FWFC_Block& Block)
{
	// Combine hash values of the block's members
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(Block.StaticMesh));
	Hash = HashCombine(Hash, GetTypeHash(Block.Rotation));
	return Hash;
}


// The UObject class that will hold a FWFC_Block instance
UCLASS(BlueprintType, Blueprintable)
class PROCEDURALALGORITHMS_API UWFC_BlockAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UWFC_BlockAsset() = default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
	FWFC_Block Block;
};
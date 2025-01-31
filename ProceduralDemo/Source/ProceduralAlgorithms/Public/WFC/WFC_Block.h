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

	FWFC_Block(UStaticMesh* StaticMesh, const TArray<FWFC_Socket>& Sockets, int Rotation = 0, int Weight = 1) :
		StaticMesh(StaticMesh),
		Rotation(Rotation),
		Weight(Weight),
		SocketFront(Sockets[0]),
		SocketBack(Sockets[1]),
		SocketRight(Sockets[2]),
		SocketLeft(Sockets[3]),
		SocketUp(Sockets[4]),
		SocketDown(Sockets[5])
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Block")
	UStaticMesh* StaticMesh = nullptr;

	// 0, 90, 180, 270
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Block")
	int Rotation = 0; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
	int Weight = 1;

	float Probability = 0.0f;
	float PLogP = 0.0f;

	// Sockets
	UPROPERTY(BlueprintReadWrite, Category = "Block")
	FWFC_Socket SocketFront;

	UPROPERTY(BlueprintReadWrite, Category = "Block")
	FWFC_Socket SocketBack;

	UPROPERTY(BlueprintReadWrite, Category = "Block")
	FWFC_Socket SocketRight;

	UPROPERTY(BlueprintReadWrite, Category = "Block")
	FWFC_Socket SocketLeft;

	UPROPERTY(BlueprintReadWrite, Category = "Block")
	FWFC_Socket SocketUp;

	UPROPERTY(BlueprintReadWrite, Category = "Block")
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
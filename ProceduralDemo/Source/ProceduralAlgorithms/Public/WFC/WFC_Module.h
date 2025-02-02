#pragma once

#include "WFC_Socket.h"
#include "CoreMinimal.h"
#include "WFC_Module.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct PROCEDURALALGORITHMS_API FWFC_Module
{
	GENERATED_BODY()

public:
	FWFC_Module() = default;

	FWFC_Module(UStaticMesh* StaticMesh, const TArray<FWFC_Socket>& Sockets, int Rotation = 0, int Weight = 1) :
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

	bool IsCompatibleWith(const FWFC_Module& NeighborModule, const FIntVector& Direction) const
	{
		if (Direction == FIntVector(1, 0, 0))
		{
			return SocketFront.CanConnect(NeighborModule.SocketBack);
		}
		if (Direction == FIntVector(-1, 0, 0))
		{
			return SocketBack.CanConnect(NeighborModule.SocketFront);
		}
		if (Direction == FIntVector(0, 1, 0))
		{
			return SocketRight.CanConnect(NeighborModule.SocketLeft);
		}
		if (Direction == FIntVector(0, -1, 0))
		{
			return SocketLeft.CanConnect(NeighborModule.SocketRight);
		}
		if (Direction == FIntVector(0, 0, 1))
		{
			return SocketUp.CanConnect(NeighborModule.SocketDown);
		}
		if (Direction == FIntVector(0, 0, -1))
		{
			return SocketDown.CanConnect(NeighborModule.SocketUp);
		}

		UE_LOG(LogTemp, Error, TEXT("FWFC_Module: IsCompatibleWith: Invalid direction"));
		return false;
	}

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
	bool operator==(const FWFC_Module& Other) const
	{
		return StaticMesh == Other.StaticMesh && Rotation == Other.Rotation;
	}
};

// Hash function is required for TMap to work
inline uint32 GetTypeHash(const FWFC_Module& Block)
{
	// Combine hash values of the block's members
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(Block.StaticMesh));
	Hash = HashCombine(Hash, GetTypeHash(Block.Rotation));
	return Hash;
}


// The UObject class that will hold a FWFC_Block instance
UCLASS(BlueprintType, Blueprintable)
class PROCEDURALALGORITHMS_API UWFC_ModuleAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UWFC_ModuleAsset() = default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
	FWFC_Module Module;
};
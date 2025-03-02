#pragma once

#include "CoreMinimal.h"
#include "WFC_Socket.generated.h"

USTRUCT(BlueprintType)
struct PROCEDURALALGORITHMS_API FWFC_Socket
{
	GENERATED_BODY()

public:
	// ID of the socket
	// 0 is empty socket (air)
	// 1 is fill socket (fully enclosed block that wont be visible)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Socket")
	int32 Id = 0;

	// if false, the socket is vertical
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Socket")
	bool bIsHorizontal = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Socket")
	bool bIsSymmetric = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Socket")
	bool bIsFlipped = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Socket")
	bool bIsInvariantRotation = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Socket")
	int Rotation = 0;

	FWFC_Socket() = default;

	FWFC_Socket(int32 InId, bool bInIsHorizontal, bool bInIsSymmetric, bool bInIsFlipped, bool bIsInvariantRotation, int Rotation = 0)
		: Id(InId),
		bIsHorizontal(bInIsHorizontal),
		bIsSymmetric(bInIsSymmetric),
		bIsFlipped(bInIsFlipped),
		bIsInvariantRotation(bIsInvariantRotation),
		Rotation(Rotation)
	{
	}

	// Can connect function checks if two sockets are compatible
	bool CanConnect(const FWFC_Socket& Other) const
	{
		// Check horizontal/vertical compatibility
		if (bIsHorizontal != Other.bIsHorizontal)
		{
			return false; // horizontal socket can only connect to horizontal socket
		}

		// Check ID match
		if (Id != Other.Id)
		{
			return false; // ids must match
		}

		if(bIsHorizontal)
		{
			// Check symmetry match
			if (bIsSymmetric && !Other.bIsSymmetric)
			{
				return false; // symmetric socket can only connect to symmetric socket
			}

			// Check flipped state match
			if (!bIsSymmetric && (bIsFlipped == Other.bIsFlipped))
			{
				return false; // flipped socket can only connect to non-flipped socket
			}
		}
		else // if vertical
		{
			// Rotation irrelevance check
			if (bIsInvariantRotation && !Other.bIsInvariantRotation)
			{
				return false; // irrelevant rotation socket can only connect to irrelevant rotation socket
			}

			// Check rotation match
			if(!bIsInvariantRotation)
			{
				if (Rotation != Other.Rotation)
				{
					return false; // rotation must match
				}
			}
		}

		return true;
	}

	FString ToString() const
	{
		FString str = FString::FromInt(Id);
		if (bIsSymmetric)
		{
			str += TEXT("s");
		}
		if (bIsFlipped)
		{
			str += TEXT("f");
		}
		if (bIsInvariantRotation)
		{
			str += TEXT("i");
		}
		return str;
	}
};
#pragma once

#include "CoreMinimal.h"
#include "Socket.generated.h"

USTRUCT(BlueprintType)
struct PROCEDURALALGORITHMS_API FSocketWFC
{
	GENERATED_BODY()

public:
	// ID of the socket
	// 0 is empty socket (air)
	// 1 is fill socket (fully enclosed block that wont be visible)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
	int32 Id = 0;

	// Using an enum for socket type (Horizontal/Vertical)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
	bool bIsHorizontal = false;

	// Symmetry flag for the socket
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
	bool bIsSymmetric = false;

	// Flipped state for the socket
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
	bool bIsFlipped = false;

	// Flag to mark if rotation is irrelevant for the socket
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
	bool bIsIrelevantRotation = false;

	FSocketWFC() = default;

	FSocketWFC(int32 InId, bool bInIsHorizontal, bool bInIsSymmetric, bool bInIsFlipped, bool bInIsIrelevantRotation)
		: Id(InId),
		bIsHorizontal(bInIsHorizontal),
		bIsSymmetric(bInIsSymmetric),
		bIsFlipped(bInIsFlipped),
		bIsIrelevantRotation(bInIsIrelevantRotation)
	{
	}

	// Can connect function checks if two sockets are compatible
	bool CanConnect(const FSocketWFC& Other) const
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

		// Rotation irrelevance check
		//if (bIsIrelevantRotation && !Other.bIsIrelevantRotation)
		//{
		//	return false; // irrelevant rotation socket can only connect to irrelevant rotation socket
		//}

		return true;
	}

	// ToString function for better visualization in the editor
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
		if (bIsIrelevantRotation)
		{
			str += TEXT("i");
		}
		return str;
	}
};




//TODO: use smaller int types
//TODO: use only one bool to distinguish between horizontal and vertical socket
//TODO: modify to use vertical sockets too
//namespace WFC
//{
//	struct Socket
//	{
//		int32 Id = 0;												// 0 is empty socket (air)
//		bool Horizontal = false;						// horizontal socket
//		bool Vertical = false;							// vertical socket
//		bool Symmetric = false;							// symmetric socket
//		bool Flipped = false;								// flipped socket
//		bool IrelevantRotation = false;			// irelevant rotation
//	
//		bool CanConnect(const Socket& Other) const
//		{
//			if((Horizontal && !Other.Horizontal) || (Vertical && !Other.Vertical))
//			{
//				return false; // horizontal socket can only connect to horizontal socket and vertical socket can only connect to vertical socket
//			}
//
//			if (Id != Other.Id)
//			{
//				return false; // ids must match
//			}
//
//			if (Symmetric && !Other.Symmetric)
//			{
//				return false; // symmetric socket can only connect to symmetric socket
//			}
//
//			if (Flipped && !Other.Flipped)
//			{
//				return false; // flipped socket can only connect to flipped socket
//			}
//
//			//if (IrelevantRotation && !Other.IrelevantRotation)
//			//{
//			//	return false; // irelevant rotation socket can only connect to irelevant rotation socket
//			//}
//
//			return true;
//		}
//
//
//		FString ToString() const
//		{
//			FString str = FString::FromInt(Id);
//			if (Symmetric)
//			{
//				str += TEXT("s");
//			}
//			if (Flipped)
//			{
//				str += TEXT("f");
//			}
//			if (IrelevantRotation)
//			{
//				str += TEXT("i");
//			}
//			return str;
//		}
//	};
//}
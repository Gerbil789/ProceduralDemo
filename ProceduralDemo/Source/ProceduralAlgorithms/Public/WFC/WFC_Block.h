#pragma once

#include "WFC_Socket.h"
#include "CoreMinimal.h"
#include "WFC_Block.generated.h"

USTRUCT(BlueprintType)
struct PROCEDURALALGORITHMS_API FWFC_Block
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
	UStaticMesh* StaticMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
	int Rotation = 0; // 0, 90, 180, 270

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
	int Priority = 1;

	// Sockets
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
	FWFC_Socket SocketFront;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
	FWFC_Socket SocketBack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
	FWFC_Socket SocketRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
	FWFC_Socket SocketLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
	FWFC_Socket SocketUp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
	FWFC_Socket SocketDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
	bool IsEmpty = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
	bool IsFill = false;

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
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT("%s_%s_%s_%s_%s_%s"),
			*SocketUp.ToString(),
			*SocketDown.ToString(),
			*SocketLeft.ToString(),
			*SocketRight.ToString(),
			*SocketFront.ToString(),
			*SocketBack.ToString()
		);
	}
};
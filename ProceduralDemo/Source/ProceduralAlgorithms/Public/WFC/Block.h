#pragma once

#include "Socket.h"
#include "CoreMinimal.h"
#include "Block.generated.h"

USTRUCT(BlueprintType)
struct PROCEDURALALGORITHMS_API FBlockWFC
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
	UStaticMesh* StaticMesh = nullptr;

	int Rotation = 0; // 0, 90, 180, 270

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
	int Priority = 1;

	// Sockets
	FSocketWFC SocketFront;
	FSocketWFC SocketBack;
	FSocketWFC SocketRight;
	FSocketWFC SocketLeft;
	FSocketWFC SocketUp;
	FSocketWFC SocketDown;

	bool IsEmpty = false;
	bool IsFill = false;

	FBlockWFC() = default;

	FBlockWFC(UStaticMesh* StaticMesh, const TArray<FSocketWFC>& Sockets, int Rotation = 0, int Priority = 1) :
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
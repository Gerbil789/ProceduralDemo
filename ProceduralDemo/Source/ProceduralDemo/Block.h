#pragma once
#include "CoreMinimal.h"
#include "Utility.h"
#include "BlockActor.h"

struct FWFCSocket
{
	virtual auto ToString() const->FString = 0; // Pure virtual function
};

struct FWFCSocketHorizontal : public FWFCSocket
{
	int32 Connector;
	bool Symmetric = false;
	bool Flipped = false;

	FWFCSocketHorizontal() : Connector(-1), Symmetric(false), Flipped(false) {}

	bool operator==(const FWFCSocketHorizontal& other) const
	{
		if (Connector == other.Connector) {
			if (Symmetric) {
				if (other.Symmetric) {
					return true;
				}
			}else if (Flipped) {
				if (!other.Flipped) {
					return true;
				}
			}
			else if (!Flipped) {
				if (other.Flipped) {
					return true;
				}
			}
		}

		return false;
	}

	auto ToString() const -> FString
	{
		FString str = FString::FromInt(Connector);
		if (Symmetric)
		{
			str += TEXT("s");
		}
		if (Flipped)
		{
			str += TEXT("f");
		}
		return str;
	}
};

struct FWFCSocketVertical : public FWFCSocket
{
	int32 Connector;
	bool IrelevantRotation = false;
	int Rotation = 0;

	FWFCSocketVertical() : Connector(-1), IrelevantRotation(false), Rotation(0) {}

	bool operator==(const FWFCSocketVertical& other) const
	{
		if(Connector == other.Connector) {
			if (IrelevantRotation) {
				if (other.IrelevantRotation) {
					return true;
				}
			}
			else if (Rotation == other.Rotation) {
				return true;
			}
		}

		return false;
	}

	auto ToString() const -> FString
	{
		FString str = FString::FromInt(Connector);
		if (IrelevantRotation)
		{
			str += TEXT("i");
		}
		return str;
	}
};

class Block
{
public:
	int MeshId = 0;
	int Rotation = 0;
	int Priority = 1;
	FString Name = "";
	
	FWFCSocketVertical Top = FWFCSocketVertical();
	FWFCSocketVertical Bottom = FWFCSocketVertical();
	FWFCSocketHorizontal Left = FWFCSocketHorizontal();
	FWFCSocketHorizontal Right = FWFCSocketHorizontal();
	FWFCSocketHorizontal Front = FWFCSocketHorizontal();
	FWFCSocketHorizontal Back = FWFCSocketHorizontal();

	Block() = default;

	Block(const int& meshId, const int& rotation, const int& priority, const ABlockActor& blockActor) :
		MeshId(meshId),
		Rotation(rotation),
		Priority(priority),
		Top(ParseVertical(blockActor.Top, rotation)),
		Bottom(ParseVertical(blockActor.Bottom, rotation))
	{
		if (rotation == 0) {
			Left = ParseHorizontal(blockActor.Left);
			Right = ParseHorizontal(blockActor.Right);
			Front = ParseHorizontal(blockActor.Front);
			Back = ParseHorizontal(blockActor.Back);
		}
		else if (rotation == -90) {
			Left = ParseHorizontal(blockActor.Back);
			Right = ParseHorizontal(blockActor.Front);
			Front = ParseHorizontal(blockActor.Left);
			Back = ParseHorizontal(blockActor.Right);
		}
		else if (rotation == -180) {
			Left = ParseHorizontal(blockActor.Right);
			Right = ParseHorizontal(blockActor.Left);
			Front = ParseHorizontal(blockActor.Back);
			Back = ParseHorizontal(blockActor.Front);
		}
		else if (rotation == -270) {
			Left = ParseHorizontal(blockActor.Front);
			Right = ParseHorizontal(blockActor.Back);
			Front = ParseHorizontal(blockActor.Right);
			Back = ParseHorizontal(blockActor.Left);
		}

		Name = Utility::CleanName(blockActor.GetName());

		//UE_LOG(LogTemp, Warning, TEXT("Block name: %s"), *Name);
	}

	static FWFCSocketHorizontal ParseHorizontal(const FString& str)
	{
		FWFCSocketHorizontal socket;

		if (str.EndsWith(TEXT("s"))) {
			socket.Symmetric = true;
			socket.Connector = FCString::Atoi(*str.LeftChop(1));
		}else if (str.EndsWith(TEXT("f"))) {
			socket.Flipped = true;
			socket.Connector = FCString::Atoi(*str.LeftChop(1));
		}
		else {
			socket.Connector = FCString::Atoi(*str);
		}

		return socket;
	}

	static FWFCSocketVertical ParseVertical(const FString& str, const int& rotation)
	{
		FWFCSocketVertical socket;

		if(str.EndsWith(TEXT("i"))) {
			socket.IrelevantRotation = true;
			socket.Connector = FCString::Atoi(*str.LeftChop(1));
		}
		else {
			socket.Connector = FCString::Atoi(*str);
			socket.Rotation = rotation;
		}

		return socket;
	}

	FString ToString() const
	{
		FString str = TEXT(" MeshId:") + FString::FromInt(MeshId) + TEXT(" Rot: ") + FString::FromInt(Rotation) + TEXT(" Top: ");
		str += Top.ToString() + TEXT(" Bottom: ") + Bottom.ToString() + TEXT(" Left: ");
		str += Left.ToString() + TEXT(" Right: ") + Right.ToString() + TEXT(" Front: ") + Front.ToString() + TEXT(" Back: ") + Back.ToString();
		return str;
	}

	bool operator==(const Block& other) const
	{
		return Name == other.Name &&
			Rotation == other.Rotation;
	}

	// Hash function (must be defined for TMap to work)
	friend uint32 GetTypeHash(const Block& Block)
	{
		uint32 HashCode = GetTypeHash(Block.Name);
		HashCode = HashCombine(HashCode, GetTypeHash(Block.MeshId));
		return HashCode;
	}
};
	

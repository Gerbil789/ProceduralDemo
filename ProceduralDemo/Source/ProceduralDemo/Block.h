#pragma once
#include "CoreMinimal.h"
#include "Utility.h"
#include "BlockActor.h"

struct Socket
{
	virtual auto ToString() const->FString = 0; // Pure virtual function
};

struct SocketHorizontal : public Socket
{
	int32 Connector = 0;
	bool Symmetric = false;
	bool Flipped = false;

	SocketHorizontal() {}

	SocketHorizontal(const FString& str)
	{
		if (str.EndsWith(TEXT("s"))) {
			Symmetric = true;
			Connector = FCString::Atoi(*str.LeftChop(1));
		}
		else if (str.EndsWith(TEXT("f"))) {
			Flipped = true;
			Connector = FCString::Atoi(*str.LeftChop(1));
		}
		else {
			Connector = FCString::Atoi(*str);
		}
	}

	auto operator==(const SocketHorizontal& other) const -> bool
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

struct SocketVertical : public Socket
{
	int32 Connector;
	bool IrelevantRotation = false;
	int Rotation = 0;

	SocketVertical() {}

	SocketVertical(const FString& str, const int& rotation)  
	{
		if (str.EndsWith(TEXT("i"))) {
			IrelevantRotation = true;
			Connector = FCString::Atoi(*str.LeftChop(1));
		}
		else {
			Connector = FCString::Atoi(*str);
			Rotation = rotation;
		}
	}

	auto operator==(const SocketVertical& other) const -> bool
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
	
	SocketVertical Top = SocketVertical();
	SocketVertical Bottom = SocketVertical();
	SocketHorizontal Left = SocketHorizontal();
	SocketHorizontal Right = SocketHorizontal();
	SocketHorizontal Front = SocketHorizontal();
	SocketHorizontal Back = SocketHorizontal();

	Block(const int& meshId, const int& rotation, const int& priority, const ABlockActor& blockActor) :
		MeshId(meshId),
		Rotation(rotation),
		Priority(priority),
		Top(SocketVertical(blockActor.Top, rotation)),
		Bottom(SocketVertical(blockActor.Bottom, rotation))
	{
		if (rotation == 0) {
			Left = SocketHorizontal(blockActor.Left);
			Right = SocketHorizontal(blockActor.Right);
			Front = SocketHorizontal(blockActor.Front);
			Back = SocketHorizontal(blockActor.Back);
		}
		else if (rotation == -90) {
			Left = SocketHorizontal(blockActor.Back);
			Right = SocketHorizontal(blockActor.Front);
			Front = SocketHorizontal(blockActor.Left);
			Back = SocketHorizontal(blockActor.Right);
		}
		else if (rotation == -180) {
			Left = SocketHorizontal(blockActor.Right);
			Right = SocketHorizontal(blockActor.Left);
			Front = SocketHorizontal(blockActor.Back);
			Back = SocketHorizontal(blockActor.Front);
		}
		else if (rotation == -270) {
			Left = SocketHorizontal(blockActor.Front);
			Right = SocketHorizontal(blockActor.Back);
			Front = SocketHorizontal(blockActor.Right);
			Back = SocketHorizontal(blockActor.Left);
		}

		Name = Utility::CleanName(blockActor.GetName());

		//UE_LOG(LogTemp, Warning, TEXT("Block name: %s"), *Name);
	}

	FString ToString() const
	{
		FString str = TEXT(" MeshId:") + FString::FromInt(MeshId) + TEXT(" Rot: ") + FString::FromInt(Rotation) + TEXT(" Top: ");
		str += Top.ToString() + TEXT(" Bottom: ") + Bottom.ToString() + TEXT(" Left: ");
		str += Left.ToString() + TEXT(" Right: ") + Right.ToString() + TEXT(" Front: ") + Front.ToString() + TEXT(" Back: ") + Back.ToString();
		return str;
	}

	auto operator==(const Block& other) const -> bool
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

	bool IsValidNeighbor(const Block& other, const FIntVector& direction)
	{
		if (direction.X == 1) {
			return (this->Left == other.Right);
		}
		else if (direction.X == -1) {
			return (this->Right == other.Left);
		}
		else if (direction.Y == 1) {
			return (this->Back == other.Front);
		}
		else if (direction.Y == -1) {
			return (this->Front == other.Back);
		}
		else if (direction.Z == 1) {
			return (this->Bottom == other.Top);
		}
		else if (direction.Z == -1) {
			return (this->Top == other.Bottom);
		}

		throw std::invalid_argument("Invalid direction (expected horizontal)");
	}

};
	

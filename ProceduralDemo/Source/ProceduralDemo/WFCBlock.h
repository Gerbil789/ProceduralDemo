#pragma once
#include "CoreMinimal.h"
#include "Utility.h"
#include "WFC_Base.h"

struct FWFCSocket
{
	virtual FString ToString() const = 0; // Pure virtual function
};

struct FWFCSocketHorizontal : public FWFCSocket
{
	int32 Connector;
	bool Symmetric;
	bool Flipped;

	FWFCSocketHorizontal() : Connector(0), Symmetric(false), Flipped(false) {}

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

	FString ToString() const
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
	bool IrelevantRotation;
	int Rotation;

	FWFCSocketVertical() : Connector(0), IrelevantRotation(false), Rotation(0) {}

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

	FString ToString() const
	{
		FString str = FString::FromInt(Connector);
		if (IrelevantRotation)
		{
			str += TEXT("i");
		}
		return str;
	}
};

class WFCBlock
{
public:
	int MeshId;
	int Rotation = 0;
	int Priority = 1;
	FString Name;
	
	FWFCSocketVertical Top;
	FWFCSocketVertical Bottom;
	FWFCSocketHorizontal Left;
	FWFCSocketHorizontal Right;
	FWFCSocketHorizontal Front;
	FWFCSocketHorizontal Back;
	BlockType Type = BlockType::EMPTY;

	WFCBlock(const int& meshId, const int& rotation, const int& priority, const AWFC_Base& base) :
		MeshId(meshId),
		Rotation(rotation),
		Priority(priority),
		Top(ParseVertical(base.Top, rotation)),
		Bottom(ParseVertical(base.Bottom, rotation)),
		Type(base.Type)
	{
		if (rotation == 0) {
			Left = ParseHorizontal(base.Left);
			Right = ParseHorizontal(base.Right);
			Front = ParseHorizontal(base.Front);
			Back = ParseHorizontal(base.Back);
		}
		else if (rotation == -90) {
			Left = ParseHorizontal(base.Back);
			Right = ParseHorizontal(base.Front);
			Front = ParseHorizontal(base.Left);
			Back = ParseHorizontal(base.Right);
		}
		else if (rotation == -180) {
			Left = ParseHorizontal(base.Right);
			Right = ParseHorizontal(base.Left);
			Front = ParseHorizontal(base.Back);
			Back = ParseHorizontal(base.Front);
		}
		else if (rotation == -270) {
			Left = ParseHorizontal(base.Front);
			Right = ParseHorizontal(base.Back);
			Front = ParseHorizontal(base.Right);
			Back = ParseHorizontal(base.Left);
		}

		Name = Utility::CleanName(base.GetName());

		UE_LOG(LogTemp, Warning, TEXT("Block name: %s"), *Name);
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

	bool operator==(const WFCBlock& other) const
	{
		return MeshId == other.MeshId && 
			Rotation == other.Rotation &&
			Top == other.Top && 
			Bottom == other.Bottom &&
			Left == other.Left &&
			Right == other.Right &&
			Front == other.Front && 
			Back == other.Back;
	}
};
	

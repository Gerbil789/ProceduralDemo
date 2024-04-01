#pragma once
#include "CoreMinimal.h"

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
	FString Mesh;
	int Rotation = 0;

	FWFCSocketHorizontal Left;
	FWFCSocketHorizontal Right;
	FWFCSocketHorizontal Front;
	FWFCSocketHorizontal Back;

	FWFCSocketVertical Top;
	FWFCSocketVertical Bottom;

	WFCBlock(FString mesh, const int& rotation, const FString& top, const FString& bottom, const FString& left, const FString& right, const FString& front, const FString& back) :
		Mesh(mesh),
		Rotation(rotation),
		Top(ParseVertical(top, rotation)),
		Bottom(ParseVertical(bottom, rotation)),
		Left(ParseHorizontal(left)),
		Right(ParseHorizontal(right)),
		Front(ParseHorizontal(front)),
		Back(ParseHorizontal(back))
	{}

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
		FString str = Mesh + TEXT(" Rot: ") + FString::FromInt(Rotation) + TEXT(" Top: ");
		str += Top.ToString() + TEXT(" Bottom: ") + Bottom.ToString() + TEXT(" Left: ");
		str += Left.ToString() + TEXT(" Right: ") + Right.ToString() + TEXT(" Front: ") + Front.ToString() + TEXT(" Back: ") + Back.ToString();
		return str;
	}
};
	

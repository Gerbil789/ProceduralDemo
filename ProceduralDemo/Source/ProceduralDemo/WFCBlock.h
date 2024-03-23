#pragma once
#include "CoreMinimal.h"

struct FWFCSocketHorizontal
{
	int32 Connector;
	bool Symmetric;
	bool Flipped;

	FWFCSocketHorizontal() : Connector(0), Symmetric(false), Flipped(false) {}

	bool operator==(const FWFCSocketHorizontal& other) const
	{
		return Connector == other.Connector && ((Symmetric == true && Symmetric == other.Symmetric) || Flipped != other.Flipped);
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

struct FWFCSocketVertical
{
	int32 Connector;
	bool IrelevantRotation;

	FWFCSocketVertical() : Connector(0), IrelevantRotation(false) {}

	bool operator==(const FWFCSocketVertical& other) const
	{
		return Connector == other.Connector;
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
		Top(ParseVertical(top)),
		Bottom(ParseVertical(bottom)),
		Left(ParseHorizontal(left)),
		Right(ParseHorizontal(right)),
		Front(ParseHorizontal(front)),
		Back(ParseHorizontal(back))
	{}

	static FWFCSocketHorizontal ParseHorizontal(const FString& str)
	{
		FWFCSocketHorizontal socket;
		socket.Connector = FCString::Atoi(*str.LeftChop(2));
		socket.Symmetric = str.EndsWith(TEXT("s"));
		socket.Flipped = str.EndsWith(TEXT("f"));
		return socket;
	}

	static FWFCSocketVertical ParseVertical(const FString& str)
	{
		FWFCSocketVertical socket;
		socket.Connector = FCString::Atoi(*str.LeftChop(1));
		socket.IrelevantRotation = str.EndsWith(TEXT("i"));
		return socket;
	}

FString ToString() const
	{
		FString str = Mesh + TEXT(" Rot:") + FString::FromInt(Rotation) + TEXT(" Top:");
		str += Top.ToString() + TEXT(" Bottom:") + Bottom.ToString() + TEXT(" Left:");
		str += Left.ToString() + TEXT(" Right") + Right.ToString() + TEXT(" Front") + Front.ToString() + TEXT(" Back") + Back.ToString();
		return str;
	}
};
	

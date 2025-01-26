#include "WFC/WFC_BlockPreviewActor.h"

AWFC_BlockPreviewActor::AWFC_BlockPreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	WireframeComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WireframeComponent"));
	WireframeComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WireframeComponent->SetupAttachment(RootComponent);

	NameComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NameComponent"));
	NameComponent->SetupAttachment(RootComponent);
	NameComponent->SetText(FText());
	NameComponent->SetHorizontalAlignment(EHTA_Center);
	NameComponent->SetVerticalAlignment(EVRTA_TextCenter);
	NameComponent->SetWorldSize(60.0f);
	NameComponent->SetTextRenderColor(FColor::Red);
	NameComponent->SetWorldRotation(FRotator(0, 0, 0));


	const int TextOffset = 120;
	const TArray<FVector> TextPositions = {
		FVector(TextOffset, 0, 0),		// front
		FVector(-TextOffset, 0, 0),		// back
		FVector(0, TextOffset, 0),		// right
		FVector(0, -TextOffset, 0),		// left
		FVector(0, 0, TextOffset),		// up
		FVector(0, 0, -TextOffset)		// down
	};

	const TArray<FString> TextNames = {
		TEXT("Front"),
		TEXT("Back"),
		TEXT("Right"),
		TEXT("Left"),
		TEXT("Up"),
		TEXT("Down")
	};

	TextComponents.Reserve(6);
	for (int32 i = 0; i < 6; i++)
	{
		UTextRenderComponent* TextComponent = CreateDefaultSubobject<UTextRenderComponent>(*TextNames[i]);
		TextComponent->SetupAttachment(RootComponent);
		TextComponent->SetRelativeLocation(TextPositions[i]);
		TextComponent->SetHorizontalAlignment(EHTA_Center);
		TextComponent->SetVerticalAlignment(EVRTA_TextCenter);
		TextComponent->SetWorldSize(50.0f);
		TextComponent->SetText(FText::FromString(TextNames[i])); 
		TextComponent->SetWorldRotation(FRotator(0, 0, 0));

		TextComponents.Add(TextComponent);
	}
}


void AWFC_BlockPreviewActor::Initialize(const FWFC_Block Block, UStaticMesh* WireframeMesh)
{
	if (!Block.IsEmpty())
	{
		if (!Block.StaticMesh || !MeshComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("StaticMesh or MeshComponent is not initialized properly."));
			return;
		}

		MeshComponent->SetStaticMesh(Block.StaticMesh);

		FRotator Rotation = FRotator(0, Block.Rotation, 0);
		MeshComponent->SetWorldRotation(Rotation);
	}
	else
	{
		NameComponent->SetText(FText::FromString("Empty"));
	}

	if (TextComponents.Num() != 6)
	{
		UE_LOG(LogTemp, Error, TEXT("TextComponents array is not initialized properly."));
		return;
	}

	TextComponents[0]->SetText(FText::FromString(Block.SocketFront.ToString()));
	TextComponents[1]->SetText(FText::FromString(Block.SocketBack.ToString()));
	TextComponents[2]->SetText(FText::FromString(Block.SocketRight.ToString()));
	TextComponents[3]->SetText(FText::FromString(Block.SocketLeft.ToString()));
	TextComponents[4]->SetText(FText::FromString(Block.SocketUp.ToString()));
	TextComponents[5]->SetText(FText::FromString(Block.SocketDown.ToString()));

	// optional wireframe mesh
	if(WireframeMesh)
	{
		WireframeComponent->SetStaticMesh(WireframeMesh);
	}
}
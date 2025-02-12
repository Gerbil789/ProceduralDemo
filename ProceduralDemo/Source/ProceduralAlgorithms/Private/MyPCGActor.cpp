#include "MyPCGActor.h"
#include "Components/BoxComponent.h"

AMyPCGActor::AMyPCGActor()
{
	PrimaryActorTick.bCanEverTick = true;

	//set root component
	VolumeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("VolumeBox"));
	RootComponent = VolumeBox;
	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	PCGComponent = CreateDefaultSubobject<UPCGComponent>(TEXT("PCGComponent"));
	PCGComponent->GenerationTrigger = EPCGComponentGenerationTrigger::GenerateOnDemand;
	PCGComponent->SetGraph(PCGGraph);

}

void AMyPCGActor::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	UpdatePCG();
}

void AMyPCGActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdatePCG();
}

void AMyPCGActor::UpdatePCG()
{
	FVector CurrentPlayerPosition = PlayerPawn->GetActorLocation();
	float DistanceMoved = FVector::Dist(CurrentPlayerPosition, this->GetActorLocation());

	if (DistanceMoved >= UpdateDistanceThreshold)
	{
		//move this to player position
		this->SetActorLocation(CurrentPlayerPosition);

		UE_LOG(LogTemp, Warning, TEXT("Updating PCG"));
		PCGComponent->Cleanup();
		PCGComponent->Generate();
	}
}



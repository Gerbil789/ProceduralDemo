#include "GoalActor.h"

AGoalActor::AGoalActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

FVector AGoalActor::GetNewLocation()
{
  FVector Origin = this->GetActorLocation();

  float Distance = FMath::RandRange(MinDistance, MaxDistance);
  float Angle = FMath::RandRange(0.0f, 2 * PI);

  float RandomX = FMath::Cos(Angle) * Distance;
  float RandomY = FMath::Sin(Angle) * Distance;

  FVector NewLocation = Origin + FVector(RandomX, RandomY, 0);

  return NewLocation;
}
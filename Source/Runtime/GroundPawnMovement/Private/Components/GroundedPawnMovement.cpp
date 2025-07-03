// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/GroundedPawnMovement.h"

UGroundedPawnMovement::UGroundedPawnMovement()
{
	NavMovementProperties.bUseAccelerationForPaths = true;
}

void UGroundedPawnMovement::BeginPlay()
{
	Super::BeginPlay();

	CachedOwner = GetOwner();
}

void UGroundedPawnMovement::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	RotateTowardsMovement(DeltaTime);
}

void UGroundedPawnMovement::RotateTowardsMovement(const float DeltaTime) const
{
	if (!bRotateTowardMovement || !CachedOwner.IsValid())
	{
		return;
	}
	
	// We only care about the horizontal direction
	const FVector HorizontalVel = FVector(Velocity.X, Velocity.Y, 0.f);
	if (HorizontalVel.SizeSquared() < FMath::Square(MinOrientSpeed)) return;

	const FRotator CurrentRot = CachedOwner->GetActorRotation();
	const FRotator DesiredRot = HorizontalVel.Rotation();
	const float Speed = FMath::Max(1.f, RotationSpeed);

	const FRotator NewRot = FMath::RInterpTo(CurrentRot, DesiredRot, DeltaTime, Speed / 360.f);
	CachedOwner->SetActorRotation(NewRot);
}

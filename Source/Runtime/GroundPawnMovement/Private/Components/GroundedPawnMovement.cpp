// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/GroundedPawnMovement.h"

#include "KiraHelperLibrary.h"
#include "Logging.h"

UGroundedPawnMovement::UGroundedPawnMovement()
{
	NavMovementProperties.bUseAccelerationForPaths = true;
}

void UGroundedPawnMovement::AddImpulse( FVector Impulse, bool bVelocityChange)
{
	if (Impulse.IsZero() || !CanMove() || !IsActive())
	{
		return;
	}
	
	// handle scaling by mass
	FVector FinalImpulse = Impulse;
	if (!bVelocityChange)
	{
		if (float Mass = UKiraHelperLibrary::GetMass(GetOwner());
			Mass > UE_SMALL_NUMBER)
		{
			FinalImpulse = FinalImpulse / Mass;
		}
		else
		{
			UE_LOG(LogGroundPawnMovement, Warning, TEXT("%s >> %s: Attempt to apply force to zero or negative Mass"), StringCast<TCHAR>(__FUNCTION__).Get(), *GetOwner()->GetName());
		}
	}

	PendingForces.ImpulseToApply += FinalImpulse;
}

void UGroundedPawnMovement::AddForce(FVector Force)
{
	if (Force.IsZero() || !CanMove() || !IsActive())
	{
		return;
	}

	if (const float Mass = UKiraHelperLibrary::GetMass(GetOwner());
		Mass > UE_SMALL_NUMBER)
	{
		PendingForces.ForceToApply += Force / Mass;
	}
	else
	{
		UE_LOG(LogGroundPawnMovement, Warning, TEXT("%s >> %s: Attempt to apply force to zero or negative Mass"), StringCast<TCHAR>(__FUNCTION__).Get(), *GetOwner()->GetName());
	}
}

void UGroundedPawnMovement::ClearAccumulatedForces()
{
	PendingForces.ImpulseToApply = FVector::ZeroVector;
	PendingForces.ForceToApply = FVector::ZeroVector;
}

bool UGroundedPawnMovement::CanMove() const
{
	return MovementState.bCanWalk || MovementState.bCanSwim || MovementState.bCanFly;
}

void UGroundedPawnMovement::BeginPlay()
{
	Super::BeginPlay();

	CachedOwner = GetOwner();
}

void UGroundedPawnMovement::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ApplyAccumulatedForces(DeltaTime);
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

void UGroundedPawnMovement::ApplyAccumulatedForces(float DeltaSeconds)
{
	const FVector::FReal ImpulseToApplyZ = GetGravitySpaceZ(PendingForces.ImpulseToApply);
	const FVector::FReal ForceToApplyZ = GetGravitySpaceZ(PendingForces.ForceToApply);
	if (ImpulseToApplyZ != 0.0 || ForceToApplyZ != 0.0)
	{
		// check to see if applied momentum is enough to overcome gravity
		if (IsMovingOnGround() && (ImpulseToApplyZ + (ForceToApplyZ * DeltaSeconds) + (GetGravityZ() * DeltaSeconds) > UE_SMALL_NUMBER))
		{
			// Falling
		}
	}

	Velocity += PendingForces.ImpulseToApply + (PendingForces.ForceToApply * DeltaSeconds);
	ClearAccumulatedForces();
}

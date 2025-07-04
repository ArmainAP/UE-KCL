// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/GroundedPawnMovement.h"

#include "KiraHelperLibrary.h"
#include "Logging.h"
#include "Misc/DataValidation.h"

#if WITH_EDITOR
EDataValidationResult UGroundedPawnMovement::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!Cast<APawn>(GetOwner()))
	{
		const FString Error = FString::Printf(TEXT("%s is not a child of %s"), *GetOwner()->GetName(), *APawn::StaticClass()->GetName());
		Context.AddError(FText::FromString(Error));
		Result = EDataValidationResult::Invalid;
	}
	
	return Result;
}
#endif

UGroundedPawnMovement::UGroundedPawnMovement()
{
	NavMovementProperties.bUseAccelerationForPaths = true;
	SetWalkableFloorZ(0.71f);
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

bool UGroundedPawnMovement::IsWalkable(const FHitResult& Hit) const
{
	if (!Hit.IsValidBlockingHit())
	{
		// No hit, or starting in penetration
		return false;
	}

	// Never walk up vertical surfaces.
	const FVector::FReal ImpactNormalZ = GetGravitySpaceZ(Hit.ImpactNormal);
	if (ImpactNormalZ < UE_KINDA_SMALL_NUMBER)
	{
		return false;
	}

	float TestWalkableZ = WalkableFloorZ;

	// See if this component overrides the walkable floor z.
	const UPrimitiveComponent* HitComponent = Hit.Component.Get();
	if (HitComponent)
	{
		const FWalkableSlopeOverride& SlopeOverride = HitComponent->GetWalkableSlopeOverride();
		TestWalkableZ = SlopeOverride.ModifyWalkableFloorZ(TestWalkableZ);
	}

	// Can't walk on this surface if it is too steep.
	if (ImpactNormalZ < TestWalkableZ)
	{
		return false;
	}

	return true;
}

void UGroundedPawnMovement::SetWalkableFloorAngle(float InWalkableFloorAngle)
{
	WalkableFloorAngle = FMath::Clamp(InWalkableFloorAngle, 0.f, 90.0f);
	WalkableFloorZ = FMath::Cos(FMath::DegreesToRadians(WalkableFloorAngle));
}

void UGroundedPawnMovement::SetWalkableFloorZ(float InWalkableFloorZ)
{
	WalkableFloorZ = FMath::Clamp(InWalkableFloorZ, 0.f, 1.f);
	WalkableFloorAngle = FMath::RadiansToDegrees(FMath::Acos(WalkableFloorZ));
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

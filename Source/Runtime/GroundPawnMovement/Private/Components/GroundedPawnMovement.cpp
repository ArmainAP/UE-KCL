// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/GroundedPawnMovement.h"

#include "KiraHelperLibrary.h"
#include "Logging.h"
#include "Misc/DataValidation.h"

#if WITH_EDITOR
EDataValidationResult UGroundedPawnMovement::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (const UBlueprintGeneratedClass* BPGC = GetTypedOuter<UBlueprintGeneratedClass>();
		BPGC && !BPGC->IsChildOf(APawn::StaticClass()))
	{
		Context.AddError(NSLOCTEXT("Validation", "NotPawnClass", "Blueprint class owning this component must derive from APawn."));
		Result = EDataValidationResult::Invalid;
	}
	
	return Result;
}
#endif

UGroundedPawnMovement::UGroundedPawnMovement()
{
	NavMovementProperties.bUseAccelerationForPaths = true;
	SetWalkableFloorZ(0.71f);

	MovementState.bCanWalk = true;
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
	PendingForces.Clear();
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

void UGroundedPawnMovement::TickComponent(
	float DeltaTime,
	enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!CanMove() || !UpdatedComponent)
	{
		return;
	}

	// Apply Gravity

	FVector PendingDelta = ComputeGravity(DeltaTime);
	PendingDelta += ComputeAccumulatedForces(DeltaTime);
	
	FHitResult Hit;
	SafeMoveUpdatedComponent(PendingDelta, RotateTowardsMovement(DeltaTime), true, Hit);

	ClearAccumulatedForces();
}

FRotator UGroundedPawnMovement::RotateTowardsMovement(const float DeltaTime) const
{
	const FRotator CurrentRot = UpdatedComponent->GetComponentRotation();
	if (!bRotateTowardMovement || !CachedOwner.IsValid())
	{
		return CurrentRot;
	}
	
	// We only care about the horizontal direction
	const FVector HorizontalVel = FVector(Velocity.X, Velocity.Y, 0.f);
	if (HorizontalVel.SizeSquared() < FMath::Square(MinOrientSpeed)) return CurrentRot;
	
	const FRotator DesiredRot = HorizontalVel.Rotation();
	const float Speed = FMath::Max(1.f, RotationSpeed);

	return FMath::RInterpTo(CurrentRot, DesiredRot, DeltaTime, Speed / 360.f);
}

FVector UGroundedPawnMovement::ComputeAccumulatedForces(float DeltaSeconds)
{
	return PendingForces.ImpulseToApply + (PendingForces.ForceToApply * DeltaSeconds);
}

// Direction-aware acceleration (always points “down”)
FVector UGroundedPawnMovement::GetGravityAccel() const
{
	const float GravityMag = FMath::Abs(GetWorld()->GetGravityZ()) * GravityScale;
	return GravityDirection * GravityMag;          // GravityDirection is normalised
}

FVector UGroundedPawnMovement::ComputeGravity(const float DeltaSeconds)
{
	return GetGravityAccel() * DeltaSeconds;
}
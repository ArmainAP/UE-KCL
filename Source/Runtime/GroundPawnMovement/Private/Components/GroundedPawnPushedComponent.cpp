// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/GroundedPawnPushedComponent.h"

#include "KiraHelperLibrary.h"
#include "Components/GroundedPawnAvoidanceSensing.h"
#include "FunctionLibrary/GroundPawnMovementHelpers.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DataValidation.h"
#include "Structs/GroundPawnSensor.h"

#if WITH_EDITOR
EDataValidationResult UGroundedPawnPushedComponent::IsDataValid(class FDataValidationContext& Context) const
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

UGroundedPawnPushedComponent* UGroundedPawnPushedComponent::Find(AActor* Actor)
{
	const APawn* Pawn = UKiraHelperLibrary::GetPawn(Actor);
	return Pawn ? Pawn->FindComponentByClass<UGroundedPawnPushedComponent>() : nullptr;
}

void UGroundedPawnPushedComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = UKiraHelperLibrary::GetPawn(GetOwner());
}

void UGroundedPawnPushedComponent::Push(const FVector PushVector)
{
	if (!OwnerPawn.IsValid()) return;
	
	const float PawnMass = FMath::Max(1, UKiraHelperLibrary::GetMass(OwnerPawn.Get()));
	const FVector ForceFactor = PushVector / PawnMass;
	if (OwnerPawn->GetVelocity().SizeSquared2D() > 0.0f)
	{
		const UMovementComponent* MovementComponent = OwnerPawn->GetMovementComponent();
		AccumulatePushForceWhileMoving(ForceFactor, MovementComponent ? MovementComponent->GetMaxSpeed() : 0.0f);
	}
	else
	{
		
		const FVector Force = ForceFactor * PawnMass * PushSettings.GetStationarySpeedFactor();
		UGroundPawnMovementHelpers::AddForce(OwnerPawn.Get(), Force);
	}
}

void UGroundedPawnPushedComponent::PushSensedComponents() const
{
	if (!PushSettings.bCanPushOthers) return;

	APawn* Pawn = UKiraHelperLibrary::GetPawn(GetOwner());
	if (!Pawn) return;

	const FVector& InputVector = Pawn->GetLastMovementInputVector();
	if (InputVector.IsNearlyZero()) return;

	UGroundedPawnAvoidanceSensing* Sensing = UGroundedPawnAvoidanceSensing::Find(Pawn);
	if (!Sensing) return;

	const FVector DesiredDir = InputVector.GetSafeNormal();

	for (const FSensorCollisionData& Hit : Sensing->RecentHits)
	{
		HandleHit(*Sensing, Hit, DesiredDir);
	}
}

void UGroundedPawnPushedComponent::Reset()
{
	PushAccumulatedForce = FVector::ZeroVector;
}


void UGroundedPawnPushedComponent::HandleHit(const UGroundedPawnAvoidanceSensing& Sensing, const FSensorCollisionData& Hit, const FVector& PusherDir) const
{
	if (!Hit.bCollided) return;
	APawn* Pawn = UKiraHelperLibrary::GetPawn(Hit.CollidedActor);
	if (!Pawn) return;

	UGroundedPawnPushedComponent* GroundedPawnPushedComponent = Find(Pawn);
	if (!GroundedPawnPushedComponent) return;

	const FVector FinalForce = ComputePushVectors(Sensing, Hit, *Pawn, PusherDir);
	if (!FinalForce.IsNearlyZero())
	{
		GroundedPawnPushedComponent->Push(FinalForce);
	}
}

FVector UGroundedPawnPushedComponent::ComputePushVectors(const UGroundedPawnAvoidanceSensing& Sensing, const FSensorCollisionData& Hit, const APawn& Pushed, const FVector& PusherDir) const
{
	/* ----------  Prep common vectors  ---------- */
	FVector PusherToPushed = Hit.CollidedActor->GetActorLocation() - OwnerPawn->GetActorLocation();
	PusherToPushed.Z = 0.f;
	PusherToPushed = PusherToPushed.GetSafeNormal();

	/* ----------  Blend between “same-side” / “opposite” push  ---------- */
	const FVector PushedInputDir = Pushed.GetVelocity().IsNearlyZero() ? Pushed.GetActorForwardVector() : Pushed.GetLastMovementInputVector().GetSafeNormal();
	const float Alignment = (FVector::DotProduct(PusherDir, PushedInputDir) + 1.f) * 0.5f;
	const FVector SameSidePush = 0.5f * (PusherToPushed + PusherDir);
	const FVector OppositePush = FVector::CrossProduct(-Hit.CollisionNormal, FVector::UpVector).GetSafeNormal();
	const FVector PushDir = FMath::Lerp(OppositePush, SameSidePush, Alignment).GetSafeNormal();

	/* ----------  Transfer vs. instant force  ---------- */
	const float AngleFactor = Sensing.Settings.GetAngleFactor(Hit);
	const float DistanceFactor = Sensing.Settings.GetDistanceFactor(Hit);

	const UMovementComponent* MovementComponent = OwnerPawn->GetMovementComponent();
	const float PushSpeedRatio = FMath::Max(1.f - PushAccumulatedForce.Length() / (MovementComponent ? MovementComponent->GetMaxSpeed() : 0.0f), 0.7f);

	const float InstantMult = DistanceFactor * AngleFactor * PushSettings.ActivePushForce * PushSpeedRatio;
	const float TransferMult = PushAccumulatedForce.Length() * PushSettings.PushForceTransferPct * DistanceFactor;

	const FVector TransferDir = FMath::Lerp(OppositePush, 0.5f * (PushAccumulatedForce.GetSafeNormal() + PusherToPushed), Alignment);
	const float SensorScale = 1.f / FMath::Max(1, Sensing.Settings.TraceCount);
	
	/* ----------  Final scaled force  ---------- */
	return (PushDir * InstantMult + TransferDir * TransferMult) * FMath::Max(1.f, UKiraHelperLibrary::GetMass(OwnerPawn.Get())) * SensorScale;
}

FVector UGroundedPawnPushedComponent::ConsumePushForce(const UWorld* World)
{
	if (!PushSettings.bCanBePushedWhileMoving)
	{
		return FVector::ZeroVector;
	}
		
	const float RandomSinePos = FMath::RandRange(0.0f, UE_PI);
	const float CurrentTime = UGameplayStatics::GetTimeSeconds(World);
	const float PushedOscillationMultiplier = 1 - PushSettings.MaxOscillationForcePct * (FMath::Sin(CurrentTime * PushSettings.OscillationPeriod) + RandomSinePos + 1) / 2;
	const FVector PushForce = PushAccumulatedForce * PushSettings.GetMovingSpeedFactor() * PushedOscillationMultiplier;
	PushAccumulatedForce = FVector::ZeroVector;
	return PushForce;
}

void UGroundedPawnPushedComponent::AccumulatePushForceWhileMoving(const FVector& AccelerationFromForceAndMass, const float Speed)
{
	if (!PushSettings.bCanBePushedWhileMoving)
	{
		return;
	}

	PushAccumulatedForce += AccelerationFromForceAndMass;
	PushAccumulatedForce = PushAccumulatedForce.GetClampedToMaxSize(Speed);
}

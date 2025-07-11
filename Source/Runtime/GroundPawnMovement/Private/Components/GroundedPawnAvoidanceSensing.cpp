// Copyright Kat Code Labs, SRL. All Rights Reserved.


#include "Components/GroundedPawnAvoidanceSensing.h"

#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "KiraHelperLibrary.h"
#include "KismetTraceUtils.h"
#include "Components/ShapeComponent.h"
#include "Data/GroundedPawnAvoidanceSensingData.h"
#include "FunctionLibrary/GroundPawnMovementHelpers.h"
#include "Misc/DataValidation.h"
#include "Navigation/PathFollowingComponent.h"

static TAutoConsoleVariable<bool> CVar_Debug_UGroundedPawnPushedComponent(TEXT("KCL.Debug.UGroundedPawnAvoidanceSensing"), false, TEXT("Global debug for UGroundedPawnAvoidanceSensing"));

#if WITH_EDITOR
EDataValidationResult UGroundedPawnAvoidanceSensing::IsDataValid(class FDataValidationContext& Context) const
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

UGroundedPawnAvoidanceSensing* UGroundedPawnAvoidanceSensing::Find(AActor* Actor)
{
	const APawn* Pawn = UKiraHelperLibrary::GetPawn(Actor);
	return Pawn ? Pawn->FindComponentByClass<UGroundedPawnAvoidanceSensing>() : nullptr;
}

UGroundedPawnAvoidanceSensing::UGroundedPawnAvoidanceSensing()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGroundedPawnAvoidanceSensing::BeginPlay()
{
	Super::BeginPlay();

	if (PresetOverride)
	{
		Settings = PresetOverride->Settings;
	}

	CachedPawn = UKiraHelperLibrary::GetPawn(GetOwner());
	if (UPathFollowingComponent* PathFollowingComponent = UKiraHelperLibrary::GetPathFollowingComponent(GetOwner()))
	{
		PathFollowingComponent->OnRequestFinished.AddLambda([this](FAIRequestID RequestID, const FPathFollowingResult& Result)
		{
			SetTempStopSensing(true);
		});
	}
}

void UGroundedPawnAvoidanceSensing::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ShouldSense())
	{
		UpdateSensors();
	}

	// Debug origin marker.
	if (CachedPawn.IsValid() && (CVar_Debug_UGroundedPawnPushedComponent.GetValueOnGameThread() || Settings.bDebugDraw))
	{
		const FVector Origin = CachedPawn->GetActorLocation() + CachedPawn->GetActorForwardVector() * Settings.ForwardOriginOffset + FVector::UpVector * Settings.UpOriginOffset;
		DrawDebugCircle(GetWorld(), Origin, Settings.SensorRadius * 4.f, 12, FColor::Emerald);
	}
}

bool UGroundedPawnAvoidanceSensing::ShouldSense() const
{
	if (!Settings.bActive || !CachedPawn.IsValid() || !CachedPawn->WasRecentlyRendered())
	{
		return false;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		return false;
	}

	if (const AActor* ViewTarget = PC->GetViewTarget())
	{
		const float CameraDistance = FVector::Distance(ViewTarget->GetActorLocation(), CachedPawn->GetActorLocation());
		return CameraDistance <= Settings.MaxDistanceCameraSensing;
	}

	// Fallback: no camera info, so rely on basic checks only.
	return true;
}

void UGroundedPawnAvoidanceSensing::UpdateSensors()
{
	RecentHits.Reset();

	const float AngleStep = Settings.TraceCount > 1 ? Settings.MaxConeAngle / (Settings.TraceCount - 1) : 0.f;

	for (int32 Index = 0; Index < Settings.TraceCount; ++Index)
	{
		const float CurrentAngle = -Settings.MaxConeAngle * 0.5f + AngleStep * Index;
		FSensorCollisionData Data;
		PerformSingleSensor(CurrentAngle, Data);

		if (Data.bCollided)
		{
			RecentHits.Add(MoveTemp(Data));
		}
	}
}

void UGroundedPawnAvoidanceSensing::PerformSingleSensor(float AngleDeg, FSensorCollisionData& OutData) const
{
	if (!CachedPawn.IsValid())
	{
		return;
	}

	const FVector Origin = CachedPawn->GetActorLocation() + CachedPawn->GetActorForwardVector() * Settings.ForwardOriginOffset + FVector::UpVector * Settings.UpOriginOffset;
	const FVector Direction = FRotator(0.f, AngleDeg, 0.f).RotateVector(CachedPawn->GetActorForwardVector()).GetSafeNormal();
	const FVector Start = Origin + Direction * Settings.NearClipping;
	const FVector End = Start + Direction * Settings.MaxSensorDistance;

	// Build query params.
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(AvoidanceSensor), false, CachedPawn.Get());
	QueryParams.AddIgnoredActors(IgnoredActors);
	FCollisionObjectQueryParams ObjParams;

	for (const auto Channel : Settings.WallFollowingChannels) { ObjParams.AddObjectTypesToQuery(Channel); }
	for (const auto Channel : Settings.AvoidanceChannels   ) { ObjParams.AddObjectTypesToQuery(Channel); }

	FHitResult Hit;
	const bool bHit = GetWorld()->SweepSingleByObjectType(Hit, Start, End, FQuat::Identity, ObjParams, FCollisionShape::MakeSphere(Settings.SensorRadius), QueryParams);

	bool bIgnored = false;
	if (bHit)
	{
		for (const FName& Tag : Settings.IgnoredActorTags)
		{
			if (Hit.GetActor() && Hit.GetActor()->ActorHasTag(Tag)) { bIgnored = true; break; }
		}
		if (!bIgnored)
		{
			for (const auto& Class : Settings.IgnoredActorClasses)
			{
				if (Hit.GetActor() && Hit.GetActor()->IsA(Class)) { bIgnored = true; break; }
			}
		}
	}
	
	if (bHit && !bIgnored && Hit.ImpactNormal.Z < UGroundPawnMovementHelpers::GetWalkableFloorZ(GetOwner()))
	{
		OutData.bCollided           = true;
		OutData.CollisionDistance   = Hit.Distance;
		OutData.CollisionNormal     = Hit.ImpactNormal;
		OutData.SensorDirection     = Direction;
		OutData.SensorAngle         = AngleDeg;
		OutData.bComponentFound     = Hit.GetComponent() != nullptr;
		OutData.HitCollisionChannel = OutData.bComponentFound ? Hit.GetComponent()->GetCollisionObjectType() : ECC_Visibility;
		OutData.CollidedActor       = Hit.GetActor();
	}
	else
	{
		OutData = FSensorCollisionData();
		OutData.SensorDirection = Direction;
		OutData.SensorAngle     = AngleDeg;
		OutData.CollisionDistance = Settings.MaxSensorDistance;
	}

	if (CVar_Debug_UGroundedPawnPushedComponent.GetValueOnGameThread() || Settings.bDebugDraw)
	{
		const FVector DrawLocation = OutData.bCollided ? Hit.Location : End;
		DrawDebugSphere(GetWorld(), DrawLocation, Settings.SensorRadius, 12, OutData.bCollided ? FColor::Yellow : FColor::Black);
	}
}

bool UGroundedPawnAvoidanceSensing::IsGoalReachable(const FVector& GoalLocation, FHitResult& OutHit) const
{
	// 1. Resolve an owner and a collision component of ANY type
	const AActor* Owner = GetOwner();
	if (!Owner) { return true; }

	const UShapeComponent* ShapeComponent = Owner->FindComponentByClass<UShapeComponent>();
	const UPrimitiveComponent* Collision = ShapeComponent ? ShapeComponent : Cast<UPrimitiveComponent>(Owner->GetRootComponent());

	if (!Collision || !Collision->IsRegistered()) { return true; }

	// 2. Build a sweep shape that matches the primitive we found
	const FCollisionShape SweepShape = Collision->GetCollisionShape();

	// 3. Prepare query parameters (unchanged except for base Actor pointer)
	FCollisionQueryParams Params(SCENE_QUERY_STAT(ReachTest), false, Owner);
	Params.AddIgnoredActors(IgnoredActors);

	FCollisionObjectQueryParams ObjParams;
	for (const auto Channel : Settings.WallFollowingChannels)
	{
		ObjParams.AddObjectTypesToQuery(Channel);
	}

	// 4. Perform the sweep
	const FVector Start = Owner->GetActorLocation();
	const FVector End   = GoalLocation + FVector::UpVector * SweepShape.GetCapsuleHalfHeight();

	const bool bHit = GetWorld()->SweepSingleByObjectType(
		OutHit, Start, End, FQuat::Identity, ObjParams, SweepShape, Params);

	// 5. Filter out hits that should be ignored
	bool bIgnored = false;
	if (bHit && OutHit.GetActor())
	{
		for (const FName& Tag : Settings.IgnoredActorTags)
		{
			if (OutHit.GetActor()->ActorHasTag(Tag)) { bIgnored = true; break; }
		}
		if (!bIgnored)
		{
			for (const auto& Class : Settings.IgnoredActorClasses)
			{
				if (OutHit.GetActor()->IsA(Class)) { bIgnored = true; break; }
			}
		}
	}

	// 6. Optional debug draw (works for all shapes)
	if (Settings.bDebugDraw)
	{
		DrawDebugCapsuleTraceSingle(
			GetWorld(), Start, End,
			SweepShape.IsCapsule() ? SweepShape.GetCapsuleRadius() : SweepShape.GetSphereRadius(),
			SweepShape.IsCapsule() ? SweepShape.GetCapsuleHalfHeight() : 0.f,
			EDrawDebugTrace::ForDuration, bHit && !bIgnored, OutHit, FColor::White, FColor::Yellow, PrimaryComponentTick.TickInterval);
	}

	return !(bHit && !bIgnored);
}

FVector UGroundedPawnAvoidanceSensing::ComputeAvoidanceDirection() const
{
	FVector SumNormal  = FVector::ZeroVector;
	FVector SumTangent = FVector::ZeroVector;
	int32   Contrib    = 0;

	for (const FSensorCollisionData& HitData : RecentHits)
	{
		if (!Settings.AvoidanceChannels.Contains(HitData.HitCollisionChannel)) { continue; }

		const float AngleW = Settings.GetAngleFactor(HitData);
		const float DistW  = Settings.GetDistanceFactor(HitData);

		SumNormal  += HitData.CollisionNormal * AngleW * DistW;
		SumTangent += FVector::CrossProduct(HitData.CollisionNormal, FVector::UpVector).GetSafeNormal() * AngleW * DistW;
		++Contrib;
	}

	if (Contrib == 0) { return FVector::ZeroVector; }
	return (SumNormal * 0.4f + SumTangent * 0.6f) / Contrib;
}

// ---------------------------------------------------------------------------
//  Public API helpers
// ---------------------------------------------------------------------------

void UGroundedPawnAvoidanceSensing::SetTempStopSensing(bool bShouldStop)
{
	Settings.bActive = !bShouldStop;
}

bool UGroundedPawnAvoidanceSensing::IsAnythingDetected() const
{
	for (const FSensorCollisionData& HitData : RecentHits)
	{
		if (HitData.bCollided)
		{
			return true;
		}
	}
	return !bGoalReachable;
}

float UGroundedPawnAvoidanceSensing::GetPawnAvoidanceSlowDownWeight() const
{
	float WeightSum = 0.f;
	int32 Count     = 0;

	for (const FSensorCollisionData& HitData : RecentHits)
	{
		if (HitData.HitCollisionChannel == ECC_Pawn)
		{
			WeightSum += Settings.GetAngleFactor(HitData) * Settings.GetDistanceFactor(HitData);
			++Count;
		}
	}

	return Count > 0 ? WeightSum / Count : 0.f;
}

FVector UGroundedPawnAvoidanceSensing::GetWallFollowingDirection() const
{
	FVector Tangent = FVector::ZeroVector;
	int32   Count   = 0;

	for (const FSensorCollisionData& HitData : RecentHits)
	{
		if (Settings.WallFollowingChannels.Contains(HitData.HitCollisionChannel) && HitData.CollidedActor == ReachTestResult.GetActor())
		{
			Tangent += FVector::CrossProduct(HitData.CollisionNormal, FVector::UpVector).GetSafeNormal() * Settings.GetAngleFactor(HitData);
			++Count;
		}
	}

	if (Count == 0)
	{
		Tangent = FVector::CrossProduct(ReachTestResult.ImpactNormal, FVector::UpVector).GetSafeNormal();
		Count = 1;
	}

	return Tangent / Count;
}

FVector UGroundedPawnAvoidanceSensing::ModifyAITrajectoryForAvoidance(const FVector& CurrentMoveVector, float DeltaTime, const FVector& CurrentTarget)
{
	bGoalReachable = IsGoalReachable(CurrentTarget, ReachTestResult);
	
	if (!ShouldSense()) { return CurrentMoveVector; }
	
	// Wall‑following heuristics.
	const bool bStartWallFollow = !bGoalReachable && ReachTestResult.Distance < Settings.WallFollowDistance;

	if (bStartWallFollow && !bIsWallFollowing)
	{
		bIsWallFollowing = true;
		const FVector TangentDir         = GetWallFollowingDirection();
		const FVector MoveToTangent      = CachedPawn->GetActorLocation() + TangentDir;
		const FVector MoveToInvTangent   = CachedPawn->GetActorLocation() - TangentDir;
		bClockwiseWallFollow = FVector::DistSquared(MoveToTangent, CurrentTarget) < FVector::DistSquared(MoveToInvTangent, CurrentTarget);
	}
	else if (!bStartWallFollow && bIsWallFollowing)
	{
		bIsWallFollowing = false;
	}

	if (bIsWallFollowing)
	{
		FVector TangentDir = GetWallFollowingDirection();
		if (!bClockwiseWallFollow) { TangentDir *= -1.f; }
		TangentDir = TangentDir.GetSafeNormal();
		float Weight = 1.f - FMath::Clamp(ReachTestResult.Distance / Settings.WallFollowDistance, 0.f, 1.f);
		Weight = Settings.DistanceWeightCurve.GetRichCurveConst()->Eval(Weight);

		if (Settings.bDebugDraw)
		{
			DrawDebugLine(GetWorld(), CachedPawn->GetActorLocation() + FVector::UpVector * 50.f, CachedPawn->GetActorLocation() + FVector::UpVector * 35.f + TangentDir * 100.f, FColor::Orange);
		}
		return TangentDir * Weight + CurrentMoveVector * (1.f - Weight);
	}

	// Regular pawn / static avoidance.
	const FVector Avoidance = ComputeAvoidanceDirection() * Settings.AvoidanceForceMultiplier;
	SmoothedAvoidanceVector = FMath::VInterpTo(SmoothedAvoidanceVector, Avoidance, DeltaTime, Settings.AvoidanceInterpSpeed);

	FVector Result = CurrentMoveVector + SmoothedAvoidanceVector;
	Result.Normalize();
	return Result;
}

float UGroundedPawnAvoidanceSensing::GetAngleFactor(const FSensorCollisionData& SensorData) const
{
	return Settings.GetAngleFactor(SensorData);
}

float UGroundedPawnAvoidanceSensing::GetDistanceFactor(const FSensorCollisionData& SensorData) const
{
	return Settings.GetDistanceFactor(SensorData);
}

float UGroundedPawnAvoidanceSensing::GetSensorScale() const
{
	return 1 / FMath::Max(1, Settings.TraceCount);
}

bool UGroundedPawnAvoidanceSensing::IsInSensingDistanceFromGoal(const float Distance) const
{
	return Distance > Settings.StopSensingDistanceFromGoal;
}

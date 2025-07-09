// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/GroundPathFollowingComponent.h"

#include "AIConfig.h"
#include "AIController.h"
#include "KiraHelperLibrary.h"
#include "NavigationSystem.h"
#include "Components/GroundedPawnPushedComponent.h"
#include "Components/SplineComponent.h"
#include "FunctionLibrary/GroundPawnMovementHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/DataValidation.h"

#if WITH_EDITOR
EDataValidationResult UGroundPathFollowingComponent::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (const UBlueprintGeneratedClass* BPGC = GetTypedOuter<UBlueprintGeneratedClass>();
		BPGC && !BPGC->IsChildOf(AAIController::StaticClass()))
	{
		Context.AddError(NSLOCTEXT("Validation", "NotAICClass", "Blueprint class owning this component must derive from AAIController."));
		Result = EDataValidationResult::Invalid;
	}
	
	return Result;
}
#endif

void UGroundPathFollowingComponent::BeginPlay()
{
	Super::BeginPlay();

	SplineComponent = Cast<USplineComponent>(GetOwner()->AddComponentByClass(USplineComponent::StaticClass(), true, FTransform(), false));
	SplineComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

FVector UGroundPathFollowingComponent::GetMoveFocus(bool bAllowStrafe) const
{
	FVector MoveFocus;

	if (bAllowStrafe && DestinationActor.IsValid())
	{
		MoveFocus = DestinationActor->GetActorLocation();
	}
	else if (SplineComponent)
	{
		MoveFocus = FocusPoint;
	}
	else
	{
		return Super::GetMoveFocus(bAllowStrafe);
	}

	return MoveFocus;
}

void UGroundPathFollowingComponent::UpdateCachedComponents()
{
	Super::UpdateCachedComponents();

	const AActor* Actor = Cast<AActor>(GetOwner());
	OldSegmentDirection = Actor->GetActorForwardVector();
}

void UGroundPathFollowingComponent::FollowPathSegment(float DeltaTime)
{
	if (!Path.IsValid() || !NavMovementInterface.IsValid())
	{
		return;
	}

	ForceRegenerateSplineRepath = IsNavmeshBlocked();
	if (ForceRegenerateSplineRepath)
	{
		TriggerRepath();
		return;
	}

	bIsDecelerating = false;
	if (SplineComponent)
	{
		ExecuteFollowPathSegment(DeltaTime);
	}
	else
	{
		Super::FollowPathSegment(DeltaTime);
	}
}

void UGroundPathFollowingComponent::OnPathFinished(const FPathFollowingResult& Result)
{
	Super::OnPathFinished(Result);
	
	if (!Result.IsInterrupted())
	{
		SpeedVariation.ResetSpeed();
		CurrentDistanceOnSpline = 0;
	}

	CurrentMoveVector = FVector::ZeroVector;
	OldSecondTangent = FVector::ZeroVector;
	CurrentTrajectoryCurvatureReduction = 1.0f;
}

void UGroundPathFollowingComponent::SetMoveSegment(int32 SegmentStartIndex)
{
	Super::SetMoveSegment(SegmentStartIndex);

	if (!SplineComponent || !NavMovementInterface.IsValid() || !Path.IsValid())
	{
		return;
	}

	if (const TArray<FNavPathPoint>& Points = Path->GetPathPoints();
		!Points.IsValidIndex(SegmentStartIndex) || !Points.IsValidIndex(SegmentStartIndex + 1))
	{
		return;
	}

	const FVector CurrentTargetLocal = GetCurrentTargetLocation();
	const FVector PreviousTarget     = Path->GetPathPoints()[MoveSegmentStartIndex];
	if (ShouldRegenerateSpline(PreviousTarget, CurrentTargetLocal))
	{
		RegenerateSpline(SegmentStartIndex);
	}
	else if (SplineComponent->GetNumberOfSplinePoints() >= 1)
	{
		SplineComponent->SetLocationAtSplinePoint(1, CurrentTargetLocal, ESplineCoordinateSpace::World, true);
	}
	
	LastValidEndPointSpline = CurrentTargetLocal;
}

bool UGroundPathFollowingComponent::ShouldDecelerate() const
{
	return NavMovementInterface.IsValid() && NavMovementInterface->UseAccelerationForPathFollowing()
		&& MoveSegmentStartIndex >= DecelerationSegmentIndex && bStopMovementOnFinish;
}

bool UGroundPathFollowingComponent::IsNavmeshBlocked() const
{
	if (!Path.IsValid()) return false;
	if (Status != EPathFollowingStatus::Moving) return false;

	const UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavigationSystem) return false;

	FVector HitLocation;
	const bool bHit = NavigationSystem->NavigationRaycast(GetWorld(), GetCurrentNavLocation(), GetCurrentTargetLocation(), HitLocation);
	return bHit;
}

void UGroundPathFollowingComponent::TriggerRepath() const
{
	const FNavPathSharedPtr CurrentPath = GetPath();
	if (!CurrentPath.IsValid())
	{
		return;
	}

	if (ANavigationData* NavData = CurrentPath->GetNavigationDataUsed())
	{
		NavData->RequestRePath(CurrentPath, ENavPathUpdateType::GoalMoved);
	}
}

FVector UGroundPathFollowingComponent::GetPointOnSpline(const float Distance, const float OverrideZ) const
{
	FVector ReturnPointOnSpline = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

	if (Trajectory.HasMinLengthCurvature())
	{
		const FSplineSample SplineSample = { Distance, SplineComponent->GetSplineLength(), SplineComponent->GetRightVectorAtTime(Distance, ESplineCoordinateSpace::World) };
		const FMoveInfluence MoveInfluence = { SpeedVariation.Get(), CurrentTrajectoryCurvatureReduction };
		SineWave.ApplyToPoint(ReturnPointOnSpline, SplineSample, MoveInfluence);
	}

	if (OverrideZ != INDEX_NONE)
	{
		UNavigationSystemV1::K2_ProjectPointToNavigation(GetWorld(), ReturnPointOnSpline, ReturnPointOnSpline, nullptr, nullptr, FVector(100.0f, 100.0f, 100.f));
	}
	
	return ReturnPointOnSpline;
}

bool UGroundPathFollowingComponent::ShouldRegenerateSpline(const FVector& InPreviousTarget, const FVector& InCurrentTarget)
{
	const float ClosestTargetInputKey = SplineComponent->FindInputKeyClosestToWorldLocation(InCurrentTarget);
	const FVector ClosestTargetPointOnSpline = SplineComponent->GetLocationAtSplineInputKey(ClosestTargetInputKey, ESplineCoordinateSpace::World);
	const float ClosestTargetDistanceOnSpline = SplineComponent->GetDistanceAlongSplineAtSplineInputKey(ClosestTargetInputKey);

	const float ClosestAIInputKey = SplineComponent->FindInputKeyClosestToWorldLocation(NavMovementInterface->GetFeetLocation());
	const float ClosestAILocationDistanceOnSpline = SplineComponent->GetDistanceAlongSplineAtSplineInputKey(ClosestAIInputKey);
	const float ClosestAILocationTimeOnSpline = SplineComponent->GetTimeAtDistanceAlongSpline(ClosestAILocationDistanceOnSpline);

	const APawn* CharacterRef = UKiraHelperLibrary::GetPawn<APawn>(GetOwner());
	
	return Trajectory.ShouldRegenerate(InCurrentTarget, InPreviousTarget, ClosestTargetPointOnSpline, LastValidEndPointSpline)
				|| CharacterRef->GetVelocity().Length() < 1.0f
				|| ClosestTargetDistanceOnSpline < CurrentDistanceOnSpline
				|| ClosestAILocationTimeOnSpline > 0.8f
				|| ForceRegenerateSplineRepath;
}

void UGroundPathFollowingComponent::UpdateFocusPoint(const float DeltaTime, const FVector& CurrentLocation, const FVector& XYVelocity)
{
	const FVector Direction = CurrentMoveVector.IsZero() ? GetOwner()->GetActorForwardVector() : CurrentMoveVector.GetSafeNormal();
	TargetFocusPoint = CurrentLocation + Direction * FAIConfig::Navigation::FocalPointDistance;

	if (FocusPoint.IsZero())
	{
		FocusPoint = TargetFocusPoint;
	}
	else
	{
		const FVector TargetFocusVector = (TargetFocusPoint - CurrentLocation).GetSafeNormal();
		const FVector CurrentFocusVector = (FocusPoint - CurrentLocation).GetSafeNormal();
		const float InterpSpeed = PathFollowingSettings.GetRotationRateToSpeedRatio(XYVelocity.Length());
		const FVector InterpolatedFocusVector = UGroundPawnMovementHelpers::InterpVectorByRotation(CurrentFocusVector, TargetFocusVector, InterpSpeed, DeltaTime).GetSafeNormal();
		FocusPoint = CurrentLocation + InterpolatedFocusVector * FAIConfig::Navigation::FocalPointDistance;
	}

	UpdateMoveFocus();
}

void UGroundPathFollowingComponent::RegenerateSpline(int32 SegmentStartIndex)
{
	constexpr float kDefaultCurvatureReduction = 0.f;
	constexpr float kInitialCurvature          = 1.f;
	constexpr float kTestDistance              = 100.f;
	constexpr int32 kSearchSteps               = 10;

	ForceRegenerateSplineRepath = false;
	GenerateSplineForCurrentSegment(SegmentStartIndex, kInitialCurvature);

	if (!IsSplinePathValid(kTestDistance))
	{
		const float Reduction = FindValidCurvatureReduction(SegmentStartIndex, kInitialCurvature, kSearchSteps, kTestDistance).Get(kDefaultCurvatureReduction);

		CurrentTrajectoryCurvatureReduction = Reduction;
		GenerateSplineForCurrentSegment(SegmentStartIndex, Reduction);
	}

	// Runtime state resets
	OldSegmentDirection = GetCurrentDirection();
	CurrentDistanceOnSpline = 0.f;
	SpeedVariation.ResetTimer();
	SineWave.UpdateSegmentIntensity();
}

void UGroundPathFollowingComponent::GenerateSplineForCurrentSegment(int32 SegmentStartIndex,
	float TangentScaleMultiplier)
{
	if (!SplineComponent)
	{
		return;
	}
	
	FVector CurrentTargetLocal = GetCurrentTargetLocation();
	const FNavigationPath* PathInstance = Path.Get();
	FVector PreviousTarget = PathInstance->GetPathPoints()[MoveSegmentStartIndex];
	SplineTargetPosition = PreviousTarget;
	const float SegmentDistance = FVector::Distance(CurrentTargetLocal, PreviousTarget);
	float NormalizedDistanceCurveInfluence = Trajectory.GetDistanceInfluence(SegmentDistance);

	FVector CurrentDirection = GetCurrentDirection();
	APawn* Pawn = UKiraHelperLibrary::GetPawn<APawn>(GetOwner());
	FVector ActorForwardDirection = Pawn ? Pawn->GetActorForwardVector().GetSafeNormal() : FVector::ZeroVector;

	SplineComponent->ClearSplinePoints();
	SplineComponent->AddSplinePoint(PreviousTarget, ESplineCoordinateSpace::World, false);
	SplineComponent->AddSplinePoint(CurrentTargetLocal, ESplineCoordinateSpace::World, false);
	SplineComponent->SetSplinePointType(0, ESplinePointType::Curve, false);
	
	FVector XYVelocity = NavMovementInterface->GetVelocityForNavMovement(); XYVelocity.Z = 0;
	float SpeedInfluence = Trajectory.GetSpeedInfluence(FMath::Clamp(XYVelocity.Length() / NavMovementInterface->GetMaxSpeedForNavMovement(), 0, 1));
	float AlphaOppositeLastTangent = Trajectory.GetOppositeBlend(FVector::DotProduct(CurrentDirection, ActorForwardDirection));

	FVector CurrentDir90Rotated(-CurrentDirection.Y, CurrentDirection.X, 0); // Rotate 90 degrees around Z-axis
	FVector CurrentDir90RotatedOppositeSide = CurrentDir90Rotated * -1.0f;
	float ActorForwardYawRotation = UKismetMathLibrary::MakeRotFromX(ActorForwardDirection).Yaw;
	float Delta90Rotated = FMath::FindDeltaAngleDegrees(UKismetMathLibrary::MakeRotFromX(CurrentDir90Rotated).Yaw, ActorForwardYawRotation);
	float Delta90RotatedOpposite = FMath::FindDeltaAngleDegrees(UKismetMathLibrary::MakeRotFromX(CurrentDir90RotatedOppositeSide).Yaw, ActorForwardYawRotation);
	FVector OppositeTurnVector = FMath::Abs(Delta90Rotated) < FMath::Abs(Delta90RotatedOpposite)
									? CurrentDir90Rotated : CurrentDir90RotatedOppositeSide;
	
	float TotalOldDirTangentInfluence = NormalizedDistanceCurveInfluence * Trajectory.OldDirCurveInfluence * SpeedInfluence;
	FVector const OldDirTangent = ActorForwardDirection * TotalOldDirTangentInfluence * (1 - AlphaOppositeLastTangent);
	float TotalOppositeTangentInfluence = Trajectory.OppositeDirCurveInfluence * SpeedInfluence * NormalizedDistanceCurveInfluence;
	FVector const OppositeDirectionTangent = OppositeTurnVector * TotalOppositeTangentInfluence * AlphaOppositeLastTangent;

	FVector FirstTangent = Trajectory.CurvatureBooster * (OldDirTangent + OppositeDirectionTangent);
	FirstTangent *= !PathFollowingSettings.HasMinTangentLength(FirstTangent);

	const FVector TangentAtSplinePoint = OldSecondTangent != FVector::ZeroVector ? OldSecondTangent : FirstTangent * TangentScaleMultiplier;
	SplineComponent->SetTangentAtSplinePoint(0, TangentAtSplinePoint, ESplineCoordinateSpace::World, false);

	if (PathInstance->GetPathPoints().IsValidIndex(SegmentStartIndex + 2))
	{
		const FVector NextTarget = PathInstance->GetPathPoints()[SegmentStartIndex + 2];
		FVector NextDirection = (NextTarget - CurrentTargetLocal).GetSafeNormal();
		NextDirection.Z = 0;
		FVector SecondTangent = Trajectory.CurvatureBooster * NextDirection * NormalizedDistanceCurveInfluence * Trajectory.OldDirCurveInfluence * SpeedInfluence * Trajectory.SecondTangentMultiplier;
		SplineComponent->SetSplinePointType(1, ESplinePointType::Curve, false);

		SecondTangent *= !PathFollowingSettings.HasMinTangentLength(SecondTangent);
		SplineComponent->SetTangentAtSplinePoint(1, SecondTangent * TangentScaleMultiplier, ESplineCoordinateSpace::World, false);
		OldSecondTangent = SplineComponent->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::World);
	}

	if (!Trajectory.HasMinLengthCurvature())
	{
		SplineComponent->SetTangentAtSplinePoint(0, FVector::ZeroVector, ESplineCoordinateSpace::World, false);
		SplineComponent->SetTangentAtSplinePoint(1, FVector::ZeroVector, ESplineCoordinateSpace::World, false);
	}

	SplineComponent->UpdateSpline();
	SplineTargetPosition = PreviousTarget;
}

TOptional<float> UGroundPathFollowingComponent::FindValidCurvatureReduction(const int32 SegmentStartIndex, const float MaxReduction,
	const int32 Steps, float DistanceCheck)
{
	const float Delta = MaxReduction / static_cast<float>(Steps);
	for (int32 Step = 1; Step <= Steps; ++Step)
	{
		const float Reduction = 1.0f - Step * Delta;
		GenerateSplineForCurrentSegment(SegmentStartIndex, Reduction);
		if (IsSplinePathValid(DistanceCheck))
		{
			return Reduction;
		}

		if (!SplineComponent)
		{
			continue;
		}

		const FVector Tangent = SplineComponent->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World);
		SplineComponent->SetTangentAtSplinePoint(0, -Tangent, ESplineCoordinateSpace::World, true);
		if (IsSplinePathValid(DistanceCheck))
		{
			return Reduction;
		}
	}

	return {};
}

bool UGroundPathFollowingComponent::IsSplinePathValid(const float DistanceIncrement) const
{
	float CurrentDistanceOnSplineLocal = 0.0f;
	float const TotalSplineLength = SplineComponent->GetSplineLength();

	FVector CurrentStartPoint = Path->GetStartLocation();
	UNavigationSystemV1::K2_ProjectPointToNavigation(GetWorld(), CurrentStartPoint, CurrentStartPoint, nullptr, nullptr);

	while (CurrentDistanceOnSplineLocal < TotalSplineLength)
	{
		CurrentDistanceOnSplineLocal += FMath::Max(DistanceIncrement, 1.0f);
		FVector CurrentEndPoint = SplineComponent->GetLocationAtDistanceAlongSpline(CurrentDistanceOnSplineLocal, ESplineCoordinateSpace::World);
		if (CurrentDistanceOnSplineLocal >= TotalSplineLength)
		{
			CurrentEndPoint = Path->GetEndLocation();
		}

		if (!UNavigationSystemV1::K2_ProjectPointToNavigation(GetWorld(), CurrentEndPoint, CurrentEndPoint, nullptr, nullptr))
		{
			return false;
		}

		if (!UKiraHelperLibrary::AreNavigationPointsConnected(this, CurrentStartPoint, CurrentEndPoint))
		{
			if (Trajectory.bDebug)
			{
				DrawDebugSphere(GetWorld(), CurrentStartPoint + FVector::UpVector * 100.0f, 7.0f, 12, FColor::Red);
				DrawDebugSphere(GetWorld(), CurrentEndPoint + FVector::UpVector * 100.0f, 7.0f, 12, FColor::Red);
			}

			return false;
		}

		CurrentStartPoint = CurrentEndPoint;
	}
	return true;
}


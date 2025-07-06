// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/GroundPathFollowingComponent.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "Components/SplineComponent.h"
#include "Misc/DataValidation.h"

#if WITH_EDITOR
EDataValidationResult UGroundPathFollowingComponent::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!Cast<AAIController>(GetOwner()))
	{
		const FString Error = FString::Printf(TEXT("%s is not a child of %s"), *GetOwner()->GetName(), *AAIController::StaticClass()->GetName());
		Context.AddError(FText::FromString(Error));
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

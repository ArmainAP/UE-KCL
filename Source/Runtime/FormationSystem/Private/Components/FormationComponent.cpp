// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/FormationComponent.h"

#include "Logging.h"
#include "KiraHelperLibrary.h"
#include "Objects/FormationContext.h"
#include "Subsystems/FormationSubsystem.h"

UFormationComponent::UFormationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UFormationComponent::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogFormationSystem, Error, TEXT("%s - World is not valid!"), StringCast<TCHAR>(__FUNCTION__).Get());
		Deactivate();
	}
	
	CachedFormationSubsystem = World->GetSubsystem<UFormationSubsystem>();
	if (!CachedFormationSubsystem)
	{
		UE_LOG(LogFormationSystem, Error, TEXT("%s - Formation Subsystem is not valid!"), StringCast<TCHAR>(__FUNCTION__).Get());
		Deactivate();
	}
}

// Called every frame
void UFormationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	PerformDistanceToGroupCheck();
}

void UFormationComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	LeaveFormation();
	
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UFormationComponent::SetupTarget(const FTransform& InTransform)
{
	if (CanRequestMovement())
	{
		TargetTransform = InTransform;
		OnMovementRequest.Broadcast(this);
	}
}

void UFormationComponent::SetMovementState(EMovementState InMovementState)
{
	MovementState = InMovementState;
	switch (MovementState) {
	case EMovementState::Stopped:
		OnMovementStopped.Broadcast(this);
		break;
	case EMovementState::Moving:
		OnMovementStarted.Broadcast(this);
		break;
	case EMovementState::Reached:
		OnReached.Broadcast(this);
		SetHasFallenBehind(false);
		break;
	}
	SetComponentTickEnabled(MovementState == EMovementState::Moving);
}

bool UFormationComponent::HasReached() const
{
	return MovementState == EMovementState::Reached;
}

FTransform UFormationComponent::GetTransform() const
{
	if (const AActor* Actor = Cast<AActor>(GetOwner()))
	{
		return Actor->GetActorTransform();
	}
	return FTransform();
}

void UFormationComponent::HandleFormationLeft(UFormationContext* OldFormationContext)
{
	FormationContext = nullptr;
	SetMovementState(EMovementState::Stopped);
	OnLeftGroup.Broadcast(this);
}

void UFormationComponent::HandleFormationJoined(UFormationContext* NewFormationContext)
{
	FormationContext = NewFormationContext;
	SetMovementState(EMovementState::Stopped);
	OnJoinedGroup.Broadcast(this);
	if (const AActor* Actor = NewFormationContext->GetFormationLead();
		Actor && (NewFormationContext->GetUnitsCount() > 1))
	{
		SetHasFallenBehind(GetDistanceTo(Actor->GetActorLocation()) > CatchUpDistanceThreshold);
	}
}

float UFormationComponent::GetDistanceToDestination() const
{
	return GetDistanceTo(GetTargetLocation());
}

bool UFormationComponent::LeaveFormation()
{
	return FormationContext ? FormationContext->RemoveUnit(this) : false; 
}

FVector UFormationComponent::GetTargetLocation() const
{
	return TargetTransform.GetLocation();
}

APawn* UFormationComponent::GetPawn() const
{
	return UKiraHelperLibrary::GetPawn(GetOwner());
}

void UFormationComponent::PerformDistanceToGroupCheck()
{
	if (!IsValid(FormationContext))
	{
		return;
	}

	if (FormationContext->GetUnitsCount() < 2)
	{
		return;
	}

	const AActor* LeadActor = FormationContext->GetFormationLead();
	if (!LeadActor || LeadActor == GetPawn())
	{
		return;
	}
	
	
	const float DistanceToGroup = GetDistanceTo(LeadActor->GetActorLocation());
	if (DistanceToGroup > FallBehindDistanceThreshold)
	{
		SetHasFallenBehind(true);
	}
	else if (DistanceToGroup < CatchUpDistanceThreshold)
	{
		SetHasFallenBehind(false);
	}
}

float UFormationComponent::GetDistanceTo(const FVector& Location) const
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return FLT_MAX;
	}

	const APawn* Pawn = GetPawn();
	if (!IsValid(Pawn))
	{
		return FLT_MAX;
	}

	float PathLength = TNumericLimits<float>::Max();
	UKiraHelperLibrary::GetNavigablePathLenght(World, Pawn->GetNavAgentLocation(), Location, PathLength);
	return PathLength;
}

void UFormationComponent::SetHasFallenBehind(const bool NewHasFallenBehind)
{
	if (bHasFallenBehind == NewHasFallenBehind)
	{
		return;
	}

	bHasFallenBehind = NewHasFallenBehind;
	if (bHasFallenBehind)
	{
		OnFallBehind.Broadcast(this);
	}
	else
	{
		OnCatchUp.Broadcast(this);
	}
}
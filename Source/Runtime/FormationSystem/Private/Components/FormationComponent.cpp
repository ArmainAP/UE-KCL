// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/FormationComponent.h"

#include "Logging.h"
#include "KiraHelperLibrary.h"
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
	if (CachedFormationSubsystem)
	{
		CachedFormationSubsystem->RemoveUnit(FormationID, this);
	}
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UFormationComponent::SetupTarget(const FTransform& InTransform)
{
	TargetTransform = InTransform;
	OnMovementRequest.Broadcast(this);
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

void UFormationComponent::HandleFormationLeft(const FName OldFormation)
{
	SetMovementState(EMovementState::Stopped);
	FormationID = NAME_None;
	OnLeftGroup.Broadcast(this);
}

void UFormationComponent::HandleFormationJoined(const FName NewFormation)
{
	SetMovementState(EMovementState::Stopped);
	FormationID = NewFormation;
	OnJoinedGroup.Broadcast(this);
	if (CachedFormationSubsystem->GetUnitsCount(FormationID) > 1)
	{
		SetHasFallenBehind(GetDistanceTo(CachedFormationSubsystem->GetFormationLeadLocation(FormationID)) > CatchUpDistanceThreshold);
	}
}

float UFormationComponent::GetDistanceToDestination() const
{
	return GetDistanceTo(GetTargetLocation());
}

bool UFormationComponent::LeaveFormation()
{
	return CachedFormationSubsystem ? CachedFormationSubsystem->RemoveUnit(FormationID, this) : false; 
}

FName UFormationComponent::GetFormationID() const
{
	return FormationID;
}

FVector UFormationComponent::GetTargetLocation() const
{
	return TargetTransform.GetLocation();
}

FVector UFormationComponent::GetFormationLeadLocation() const
{
	return CachedFormationSubsystem ?
		CachedFormationSubsystem->GetFormationLeadLocation(GetFormationID()) : FVector::ZeroVector; 
}

APawn* UFormationComponent::GetPawn() const
{
	return UKiraHelperLibrary::GetPawn(GetOwner());
}

AActor* UFormationComponent::GetFormationOwner() const
{
	return CachedFormationSubsystem ? CachedFormationSubsystem->GetFormationOwner(GetFormationID()) : nullptr;
}

void UFormationComponent::PerformDistanceToGroupCheck()
{
	if (!IsValid(CachedFormationSubsystem))
	{
		return;
	}

	if (CachedFormationSubsystem->GetUnitsCount(FormationID) < 2)
	{
		return;
	}

	if (CachedFormationSubsystem->GetFormationLead(FormationID) == GetPawn())
	{
		return;
	}
	
	const float DistanceToGroup = GetDistanceTo(CachedFormationSubsystem->GetFormationLeadLocation(FormationID));
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
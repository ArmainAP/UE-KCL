// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/FormationComponent.h"

#include "AIController.h"
#include "Logging.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
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
	
	if (GetDistanceToDestination() > DestinationDistanceThreshold)
	{
		PerformDistanceToGroupCheck();
	}
	else
	{
		StopMovement();
		OnReached.Broadcast(this);
		SetHasFallenBehind(false);
	}
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
	bReached = false;
	OnMove.Broadcast(this);
	SetComponentTickEnabled(true);
}

void UFormationComponent::StopMovement()
{
	bReached = true;
	OnStopped.Broadcast(this);
	SetComponentTickEnabled(false);
}

bool UFormationComponent::HasReached_Implementation()
{
	return bReached;
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
	FormationID = NAME_None;
	StopMovement();
	OnLeftGroup.Broadcast(this);
}

void UFormationComponent::HandleFormationJoined(const FName NewFormation)
{
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

FName UFormationComponent::GetFormationID() const
{
	return FormationID;
}

FVector UFormationComponent::GetTargetLocation() const
{
	return TargetTransform.GetLocation();
}

AAIController* UFormationComponent::GetAIController() const
{
	return UAIBlueprintHelperLibrary::GetAIController(GetOwner());
}

APawn* UFormationComponent::GetPawn() const
{
	if (const AAIController* AIController = GetAIController())
	{
		return AIController->GetPawn();
	}
	return nullptr;
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

	const UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(World, Pawn->GetNavAgentLocation(), TargetTransform.GetLocation());
	if (!IsValid(Path))
	{
		return FLT_MAX;
	}
	
	return Path->GetPathLength();
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
// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/FormationComponent.h"

#include "Logging.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Objects/FormationGroupInfo.h"
#include "Runtime/AIModule/Classes/AIController.h"

UFormationComponent::UFormationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UFormationComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerController = Cast<AAIController>(GetOwner());
	if (!IsValid(OwnerController))
	{
		UE_LOG(LogFormationSystem, Error, TEXT("%s - Owner is not an AAIController!"), StringCast<TCHAR>(__FUNCTION__).Get());
		Deactivate();
	}
}

// Called every frame
void UFormationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CachedDeltaTime = DeltaTime;
	
	PerformDistanceToGroupCheck();
	if (Execute_GetDistanceToDestination(this) < DestinationDistanceThreshold && !HandleRotation())
	{
		StopMovement_Implementation();
		OnReached.Broadcast(this);
		SetHasFallenBehind(false);
	}
}

void UFormationComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if (GroupInfo)
	{
		GroupInfo->RemoveUnit(this);
	}
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UFormationComponent::SetupTarget_Implementation(const FVector& InTargetLocation, const FRotator& InTargetRotation)
{
	if (!UNavigationSystemV1::K2_ProjectPointToNavigation(GetWorld(), InTargetLocation, TargetLocation, nullptr, nullptr))
	{
		return;
	}
	TargetRotation = InTargetRotation;
	bReached = false;
	OnMove.Broadcast(this);
	SetComponentTickEnabled(true);
}

void UFormationComponent::StopMovement_Implementation()
{
	bReached = true;
	OnStopped.Broadcast(this);
	SetComponentTickEnabled(false);
}

bool UFormationComponent::HasReached_Implementation()
{
	return bReached;
}

FTransform UFormationComponent::GetTransform_Implementation() const
{
	if (const AActor* Actor = Cast<AActor>(GetOwner()))
	{
		return Actor->GetActorTransform();
	}
	return FTransform();
}

void UFormationComponent::HandleFormationLeft_Implementation(UFormationGroupInfo* OldFormation)
{
	if (GroupInfo == nullptr)
	{
		return;
	}
	GroupInfo = nullptr;
	Execute_StopMovement(this);
	OnLeftGroup.Broadcast(OldFormation);
}

void UFormationComponent::HandleFormationJoined_Implementation(UFormationGroupInfo* NewFormation)
{
	if (GroupInfo == NewFormation)
	{
		return;
	}
	GroupInfo = NewFormation;
	OnJoinedGroup.Broadcast(NewFormation);

	SetHasFallenBehind(GetDistanceTo(GroupInfo->GetFormationLeadLocation()) > CatchUpDistanceThreshold);
}

AActor* UFormationComponent::GetActor_Implementation() const
{
	return OwnerController->GetPawn();
}

float UFormationComponent::GetDistanceToDestination_Implementation() const
{
	return GetDistanceTo(TargetLocation);
}

bool UFormationComponent::ChangeFormationGroup(UFormationGroupInfo* NewFormation)
{
	if (!IsValid(NewFormation))
	{
		return false;
	}
	
	if (GroupInfo)
	{
		GroupInfo->RemoveUnit(this);
	}
	
	return NewFormation->AddUnit(this);
}

UFormationGroupInfo* UFormationComponent::GetFormationGroupInfo()
{
	if (IsValid(GroupInfo))
	{
		return GroupInfo;
	}
	
	UFormationGroupInfo* NewGroup = NewObject<UFormationGroupInfo>(this);
	NewGroup->SetFormationDataAsset(DefaultFormationDataAsset);
	NewGroup->AddUnit(this);
	return NewGroup;
}

FVector UFormationComponent::GetTargetLocation() const
{
	return TargetLocation;
}

void UFormationComponent::PerformDistanceToGroupCheck()
{
	if (!IsValid(GroupInfo))
	{
		return;
	}

	if (GroupInfo->GetFormationLead() == OwnerController->GetPawn())
	{
		return;
	}
	
	const float DistanceToGroup = GetDistanceTo(GroupInfo->GetFormationLeadLocation());
	if (DistanceToGroup > FallBehindDistanceThreshold)
	{
		SetHasFallenBehind(true);
	}
	else if (DistanceToGroup < CatchUpDistanceThreshold)
	{
		SetHasFallenBehind(false);
	}
}

bool UFormationComponent::HandleRotation()
{
	if (DestinationRotationRate == 0.0f)
	{
		return false;
	}
	
	APawn* Pawn = Cast<APawn>(OwnerController->GetPawn());
	if (!IsValid(Pawn))
	{
		return false;
	}

	OnStopped.Broadcast(this);
	
	const FRotator PawnRotation = Pawn->GetActorRotation();
	if (PawnRotation.Equals(TargetRotation, 1e-3f))
	{
		return false;
	}

	const FRotator LerpTargetRotation = UKismetMathLibrary::RLerp(PawnRotation, TargetRotation, CachedDeltaTime * DestinationRotationRate, true);
	Pawn->SetActorRotation(LerpTargetRotation);
	return true;
}

float UFormationComponent::GetDistanceTo(const FVector& Location) const
{
	if (!IsValid(OwnerController))
	{
		return FLT_MAX;
	}
	
	const APawn* Pawn = Cast<APawn>(OwnerController->GetPawn());
	if (!IsValid(Pawn))
	{
		return FLT_MAX;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return FLT_MAX;
	}

	FVector PawnLocation = Pawn->GetNavAgentLocation();
	if (!UNavigationSystemV1::K2_ProjectPointToNavigation(World, PawnLocation, PawnLocation, nullptr, nullptr))
	{
		return FLT_MAX;
	}

	const UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(World, PawnLocation, TargetLocation);
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
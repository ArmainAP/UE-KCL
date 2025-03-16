// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/FormationComponent.h"

#include "Logging.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Objects/FormationGroupInfo.h"
#include "Runtime/AIModule/Classes/AIController.h"

UFormationComponent::UFormationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
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
	
	if (bReached)
	{
		return;
	}

	if (!HasReachedTargetLocation())
	{
		return;
	}

	if (!HandleRotation())
	{
		bReached = true;
		OnReached.Broadcast(this);
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
	TargetLocation = InTargetLocation;
	TargetRotation = InTargetRotation;
	bReached = false;
	OnMove.Broadcast(this);
}

void UFormationComponent::StopMovement_Implementation()
{
	bReached = true;
	OnStopped.Broadcast(this);
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

bool UFormationComponent::HasReachedTargetLocation()
{
	const APawn* Pawn = Cast<APawn>(OwnerController->GetPawn());
	if (!IsValid(Pawn))
	{
		return false;
	}

	FVector PawnLocation = Pawn->GetNavAgentLocation();
	if (!UNavigationSystemV1::K2_ProjectPointToNavigation(GetWorld(), PawnLocation, PawnLocation, nullptr, nullptr))
	{
		return false;
	}

	if (!UNavigationSystemV1::K2_ProjectPointToNavigation(GetWorld(), TargetLocation, TargetLocation, nullptr, nullptr))
	{
		return false;
	}

	return FVector::Distance(PawnLocation, TargetLocation) < DestinationDistanceThreshold;
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

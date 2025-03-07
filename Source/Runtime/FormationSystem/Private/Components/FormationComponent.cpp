// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/FormationComponent.h"

#include "Logging.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"
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
		OwnerController->MoveToLocation(TargetLocation, DestinationAcceptanceRadius, false);
		return;
	}

	if (!HandleRotation())
	{
		bReached = true;
		OnReached.Broadcast(this);
	}
}

void UFormationComponent::StopMovement()
{
	bReached = true;
	OnStopped.Broadcast(this);
	OwnerController->StopMovement();
}

void UFormationComponent::SetupFormation(const FVector& InTargetLocation, const FRotator& InTargetRotation)
{
	TargetLocation = InTargetLocation;
	TargetRotation = InTargetRotation;
	bReached = false;
	OnMove.Broadcast(this);
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

bool UFormationComponent::HasReached()
{
	return bReached;
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

	OwnerController->StopMovement();
	
	const FRotator PawnRotation = Pawn->GetActorRotation();
	if (PawnRotation.Equals(TargetRotation, 1e-3f))
	{
		return false;
	}

	const FRotator LerpTargetRotation = UKismetMathLibrary::RLerp(PawnRotation, TargetRotation, CachedDeltaTime * DestinationRotationRate, true);
	Pawn->SetActorRotation(LerpTargetRotation);
	return true;
}

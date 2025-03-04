// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/FormationComponent.h"

#include "Logging.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/AIModule/Classes/AIController.h"

// Sets default values for this component's properties
UFormationComponent::UFormationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
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
	
	if (MovementState != EMovementState::Moving)
	{
		return;
	}

	FVector TargetLocation;
	if (!HasReachedTargetLocation(TargetLocation))
	{
		OwnerController->MoveToLocation(TargetLocation, DestinationAcceptanceRadius, false);
	}
}

void UFormationComponent::SetMovementState(const EMovementState NewMovementState)
{
	if (MovementState == NewMovementState)
	{
		return;
	}

	MovementState = NewMovementState;
	OnMovementStateChanged.Broadcast(MovementState);
}

void UFormationComponent::SetupFormation(const FFormationUnitData& NewFormationUnitData)
{
	FormationUnitData = NewFormationUnitData;
	SetMovementState(EMovementState::Moving);
}

bool UFormationComponent::HasReachedTargetLocation(FVector& TargetLocation) const
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

	if (!IsValid(FormationUnitData.FormationLeader))
	{
		return false;
	}

	const FVector LeaderLocation = FormationUnitData.FormationLeader->GetNavAgentLocation();
	const FVector LeaderLocationOffset = FormationUnitData.FormationLeader->GetActorForwardVector().ToOrientationQuat().RotateVector(FormationUnitData.TargetLocation);
	if (!UNavigationSystemV1::K2_ProjectPointToNavigation(GetWorld(), LeaderLocation + LeaderLocationOffset, TargetLocation, nullptr, nullptr))
	{
		return false;
	}

	return FVector::Distance(PawnLocation, TargetLocation) < DestinationDistanceThreshold;
}

void UFormationComponent::OnFormationLeaderReached()
{
	if (FVector TargetLocation; !HasReachedTargetLocation(TargetLocation))
	{
		return;
	}

	if (!HandleRotation())
	{
		SetMovementState(EMovementState::Reached);
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

	if (!IsValid(FormationUnitData.FormationLeader))
	{
		return false;
	}

	OwnerController->StopMovement();
	
	const FRotator PawnRotation = Pawn->GetActorRotation();
	const FRotator TargetRotation = UKismetMathLibrary::ComposeRotators(FormationUnitData.FormationLeader->GetActorRotation(), FormationUnitData.TargetRotation);
	if (PawnRotation.Equals(TargetRotation))
	{
		Pawn->SetActorRotation(TargetRotation); // Remove?
		return false;
	}

	const FRotator LerpTargetRotation = UKismetMathLibrary::RLerp(PawnRotation, TargetRotation, CachedDeltaTime * DestinationRotationRate, true);
	Pawn->SetActorRotation(LerpTargetRotation);
	return true;
}

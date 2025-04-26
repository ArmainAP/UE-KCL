// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/FormationGroupComponent.h"

#include "Logging.h"
#include "Subsystems/FormationSubsystem.h"

UFormationGroupComponent::UFormationGroupComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1;
	FormationID = FName(FGuid::NewGuid().ToString());
}

void UFormationGroupComponent::BeginPlay()
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

	CachedFormationSubsystem->CreateGroup(FormationID, DefaultFormationDataAsset);
	if (FFormationHandleEvent* JoinEvent = CachedFormationSubsystem->OnUnitJoined(FormationID))
	{
		JoinEvent->AddLambda([this](UFormationComponent* Unit)
		{
			OnUnitJoined.Broadcast(Unit);
		});
	}

	if (FFormationHandleEvent* LeftEvent = CachedFormationSubsystem->OnUnitLeft(FormationID))
	{
		LeftEvent->AddLambda([this](UFormationComponent* Unit)
		{
			OnUnitLeft.Broadcast(Unit);
		});
	}
}

void UFormationGroupComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CachedFormationSubsystem)
	{
		const FVector& FormationDirection = bUseWorldDirection ? Direction : GetComponentRotation().RotateVector(Direction);
		CachedFormationSubsystem->MoveFormation(FormationID, GetComponentLocation(), FormationDirection);
	}
}

bool UFormationGroupComponent::GetUnits(TArray<UFormationComponent*>& Units) const
{
	return CachedFormationSubsystem->GetUnits(FormationID, Units);
}

FName UFormationGroupComponent::GetFormationID() const
{
	return FormationID;
}

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

	CachedFormationSubsystem->CreateGroup(FormationID, DefaultFormationDataAsset, GetOwner());
	CachedFormationSubsystem->OnUnitJoined.AddUObject(this, &UFormationGroupComponent::HandleUnitJoined);
	CachedFormationSubsystem->OnUnitLeft.AddUObject(this, &UFormationGroupComponent::HandleUnitLeft);
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

int UFormationGroupComponent::GetUnitsCount() const
{
	return CachedFormationSubsystem->GetUnitsCount(FormationID);
}

bool UFormationGroupComponent::AddUnit(UFormationComponent* FormationComponent) const
{
	return CachedFormationSubsystem->AddUnit(FormationID, FormationComponent);
}

bool UFormationGroupComponent::RemoveUnit(UFormationComponent* FormationComponent) const
{
	return CachedFormationSubsystem->RemoveUnit(FormationID, FormationComponent);
}

bool UFormationGroupComponent::ForEachUnit(FFormationUnitCallable Callable) const
{
	return CachedFormationSubsystem
	 ? CachedFormationSubsystem->ForEachUnit(FormationID, Callable)
	 : false;
}

bool UFormationGroupComponent::ForEachUnitBP(const FFormationUnitDynDelegate& BPDelegate) const
{
	return CachedFormationSubsystem
		 ? CachedFormationSubsystem->ForEachUnitBP(FormationID, BPDelegate)
		 : false;
}

void UFormationGroupComponent::HandleUnitJoined(const FName InFormationID, UFormationComponent* FormationComponent)
{
	if (FormationID == InFormationID)
	{
		OnUnitJoined.Broadcast(FormationComponent);	
	}
}

void UFormationGroupComponent::HandleUnitLeft(const FName InFormationID, UFormationComponent* FormationComponent)
{
	if (FormationID == InFormationID)
	{
		OnUnitLeft.Broadcast(FormationComponent);
	}
}

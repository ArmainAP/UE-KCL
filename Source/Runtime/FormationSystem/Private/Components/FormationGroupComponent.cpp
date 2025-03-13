// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/FormationGroupComponent.h"
#include "Objects/FormationGroupInfo.h"

UFormationGroupComponent::UFormationGroupComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFormationGroupComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	MoveToOwner();
}

UFormationGroupInfo* UFormationGroupComponent::GetFormationGroup()
{
	if (!IsValid(FormationGroup))
	{
		FormationGroup = NewObject<UFormationGroupInfo>();
		FormationGroup->SetFormationDataAsset(DefaultFormationDataAsset);
	}
	return FormationGroup;
}

void UFormationGroupComponent::MoveToOwner()
{
	UFormationGroupInfo* PlayerFormationGroupInfo = GetFormationGroup();
	if (!IsValid(PlayerFormationGroupInfo))
	{
		return;
	}

	if (PlayerFormationGroupInfo->GetUnitsCount())
	{
		PlayerFormationGroupInfo->MoveFormation(GetComponentLocation(), GetComponentRotation().RotateVector(Direction));
	}
}

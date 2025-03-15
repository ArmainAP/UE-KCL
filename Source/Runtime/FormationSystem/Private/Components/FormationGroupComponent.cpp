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
	UFormationGroupInfo* FormationGroupInfo = GetFormationGroup();
	if (!IsValid(FormationGroupInfo))
	{
		return;
	}

	if (FormationGroupInfo->GetUnitsCount())
	{
		FormationGroupInfo->MoveFormation(GetComponentLocation(), GetComponentRotation().RotateVector(Direction));
	}
}

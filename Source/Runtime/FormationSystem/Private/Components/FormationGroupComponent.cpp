// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/FormationGroupComponent.h"

#include "Data/FormationDataAssets/FormationDataAsset.h"
#include "Objects/FormationContext.h"
#include "Subsystems/FormationSubsystem.h"

UFormationGroupComponent::UFormationGroupComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1;
}

void UFormationGroupComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (ID.IsEmpty())
	{
		ID = FGuid::NewGuid().ToString();
	}

	Context = UFormationSubsystem::Get(GetWorld())->CreateGroup(ID, DefaultFormationDataAsset, GetOwner());
	check(Context);
}

void UFormationGroupComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Context)
	{
		const FVector& FormationDirection = bUseWorldDirection ? Direction : GetComponentRotation().RotateVector(Direction);
		Context->RequestMove(GetComponentLocation(), FormationDirection);
	}
}

void UFormationGroupComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bAutoDestroyFormation && Context)
	{
		UFormationSubsystem::Get(GetWorld())->DestroyGroup(ID);
	}
	
	Super::EndPlay(EndPlayReason);
}

FTransform UFormationGroupComponent::GetUnitWorldTransform(const int Index) const
{
	TArray<FTransform> OutTransforms;
	const FVector& FormationDirection = bUseWorldDirection ? Direction : GetComponentRotation().RotateVector(Direction);
	GetContext()->GetFormationDataAsset()->GetWorldTransforms(Index + 1, GetComponentLocation(), FormationDirection, OutTransforms);
	return OutTransforms[Index];
}

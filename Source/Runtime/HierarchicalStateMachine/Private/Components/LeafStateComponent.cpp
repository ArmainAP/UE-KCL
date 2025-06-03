// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/LeafStateComponent.h"

// Sets default values for this component's properties
ULeafStateComponent::ULeafStateComponent()
{
	bAutoActivate = false;
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	OnComponentActivated.AddUniqueDynamic(this, &ULeafStateComponent::HandleComponentActivated);
	OnComponentDeactivated.AddUniqueDynamic(this, &ULeafStateComponent::HandleComponentDeactivated);
}

void ULeafStateComponent::HandleComponentActivated(UActorComponent* Component, bool bReset)
{
	StateEnter();
}

void ULeafStateComponent::HandleComponentDeactivated(UActorComponent* Component)
{
	StateExit();
}

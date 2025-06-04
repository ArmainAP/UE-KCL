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

void ULeafStateComponent::RegisterTransition(const FGameplayTag& To, const int Index)
{
	if (AutoTransitions.IsValidIndex(Index))
	{
		AutoTransitions.Insert(To, Index);
	}
	else
	{
		AutoTransitions.Add(To);
	}
}

FGameplayTag ULeafStateComponent::GetTransitionTag(const FGameplayTag& FromTag) const
{
	for (const FGameplayTag& TransitionTag : AutoTransitions)
	{
		if (CanTransition(FromTag, TransitionTag))
		{
			return TransitionTag;
		}
	}
	return {};
}

bool ULeafStateComponent::CanTransition_Implementation(const FGameplayTag& FromTag, const FGameplayTag& ToTag) const
{
	return true;
}

void ULeafStateComponent::HandleComponentActivated(UActorComponent* Component, bool bReset)
{
	StateEnter();
}

void ULeafStateComponent::HandleComponentDeactivated(UActorComponent* Component)
{
	StateExit();
}

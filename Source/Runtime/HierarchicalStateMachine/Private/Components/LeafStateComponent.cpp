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

bool ULeafStateComponent::CanEnter_Implementation(const FGameplayTag& FromTag, const FGameplayTag& ToTag) const
{
	return !FromTag.MatchesTag(ToTag);
}

bool ULeafStateComponent::CanExit_Implementation(const FGameplayTag& CurrentTag, const FGameplayTag& ExitTag) const
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

void ULeafStateComponent::RequestExit(const EStateExitReason Reason)
{
	ExitReason = Reason;
	Deactivate();
}

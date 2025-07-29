// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/LeafStateComponent.h"

// Sets default values for this component's properties
ULeafStateComponent::ULeafStateComponent()
{
	bAutoActivate = false;
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
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

void ULeafStateComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentActivated.AddUniqueDynamic(this, &ULeafStateComponent::HandleComponentActivated);
	OnComponentDeactivated.AddUniqueDynamic(this, &ULeafStateComponent::HandleComponentDeactivated);
}

void ULeafStateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnComponentActivated.RemoveDynamic(this, &ULeafStateComponent::HandleComponentActivated);
	OnComponentDeactivated.RemoveDynamic(this, &ULeafStateComponent::HandleComponentDeactivated);
	
	Super::EndPlay(EndPlayReason);
}

void ULeafStateComponent::HandleComponentActivated(UActorComponent* Component, bool bReset)
{
	StateEnter();
	OnStateEntered.Broadcast(this);
}

void ULeafStateComponent::HandleComponentDeactivated(UActorComponent* Component)
{
	StateExit();
}

void ULeafStateComponent::RequestExit(const EStateExitReason Reason)
{
	ExitReason = Reason;
	if (!IsActive())
	{
		return;
	}

	switch (ExitReason) {
	case EStateExitReason::Canceled:
		OnStateCanceled.Broadcast(this);
		break;
	case EStateExitReason::Completed:
		OnStateCompleted.Broadcast(this);
		break;
	case EStateExitReason::Aborted:
		OnStateAborted.Broadcast(this);
		break;
	}

	Deactivate();
}

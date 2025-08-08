// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/DelayedActivationComponent.h"

UDelayedActivationComponent::UDelayedActivationComponent()
{
	bAutoActivate = false;
}

bool UDelayedActivationComponent::IsPendingActivation() const
{
	return !IsActive() && GetWorld()->GetTimerManager().TimerExists(TimerHandle);
}

bool UDelayedActivationComponent::IsPendingDeactivation() const
{
	return IsActive() && GetWorld()->GetTimerManager().TimerExists(TimerHandle);
}

void UDelayedActivationComponent::TriggerDelayedActivation()
{
	if (IsActive())
	{
		return;
	}
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,
		FTimerDelegate::CreateLambda([this]()
		{
			Activate();
		}), ActivationDelay, false);
}

void UDelayedActivationComponent::TriggerDelayedDeactivation()
{
	if (!IsActive())
	{
		return;
	}
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,
	FTimerDelegate::CreateLambda([this]()
	{
		Activate();
	}), ActivationDelay, false);
}

void UDelayedActivationComponent::ToggleDelayedActivation()
{
	if (GetWorld()->GetTimerManager().TimerExists(TimerHandle))
	{
		return;
	}

	if (IsActive())
	{
		TriggerDelayedActivation();
	}
	else
	{
		TriggerDelayedDeactivation();
	}
}
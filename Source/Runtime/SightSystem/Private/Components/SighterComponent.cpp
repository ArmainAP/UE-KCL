// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/SighterComponent.h"

#include "Subsystems/SightQueryManager.h"

// Sets default values for this component's properties
USighterComponent::USighterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
}

void USighterComponent::GetSightedComponents(const ESightedState SightedState, TArray<USightedComponent*>& OutSightedComponent) const
{
	const FSightedComponentWeakSet* SightedSet = nullptr;
	switch (SightedState)
	{
	case ESightedState::Spotted:
		SightedSet = &SpottedSightedComponents;
		break;
	case ESightedState::Perceived:
		SightedSet = &PerceivedSightedComponents;
		break;
	case ESightedState::Lost:
		SightedSet = &LostSightedComponents;
		break;
	}

	if (!SightedSet)
	{
		return;
	}

	OutSightedComponent.Reset();
	OutSightedComponent.Reserve(SightedSet->Num());
	auto Predicate = [](const TWeakObjectPtr<USightedComponent>& W){ return W.IsValid(); };
	auto Transform = [](const TWeakObjectPtr<USightedComponent>& W){ return W.Get(); };
	Algo::TransformIf(*SightedSet, OutSightedComponent, Predicate, Transform);
}

bool USighterComponent::HasSightedComponent(ESightedState SightedState, USightedComponent* SightedComponent) const
{
	if (!SightedComponent)
	{
		return false;
	}

	// Special meaning for NotPerceived: “is it absent from all lists?”
	if (SightedState == ESightedState::NotPerceived)
	{
		return !PerceivedSightedComponents.Contains(SightedComponent) &&
			!SpottedSightedComponents.Contains(SightedComponent) &&
			!LostSightedComponents.Contains(SightedComponent);
	}

	// Normal states
	TArray<USightedComponent*> SightedComponents;
	GetSightedComponents(SightedState, SightedComponents);
	return SightedComponents.Contains(SightedComponent);
}

bool USighterComponent::HasAnySightedComponent(ESightedState SightedState) const
{
	if (SightedState == ESightedState::NotPerceived)
	{
		return !PerceivedSightedComponents.Num() &&
			!SpottedSightedComponents.Num() &&
			!LostSightedComponents.Num();
	}

	TArray<USightedComponent*> SightedComponents;
	GetSightedComponents(SightedState, SightedComponents);
	return SightedComponents.Num() > 0;
}

ESightedState USighterComponent::GetStateForSightedComponent(const USightedComponent* TargetActor) const
{
	if (PerceivedSightedComponents.Contains(TargetActor))
	{
		return ESightedState::Perceived;
	}

	if (SpottedSightedComponents.Contains(TargetActor))
	{
		return ESightedState::Spotted;
	}

	if (LostSightedComponents.Contains(TargetActor))
	{
		return ESightedState::Lost;
	}

	return ESightedState::NotPerceived;
}

void USighterComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoActivate)
	{
		Activate(true);
	}
}

void USighterComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	if (USightQueryManager* SightSystem = World->GetSubsystem<USightQueryManager>())
	{
		SightSystem->RegisterSighter(this);
	}
}

void USighterComponent::Deactivate()
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	if (USightQueryManager* SightSystem = World->GetSubsystem<USightQueryManager>())
	{
		SightSystem->UnregisterSighter(this);
	}

	Super::Deactivate();
}

void USighterComponent::SpotTarget(USightedComponent* TargetActor)
{
	SpottedSightedComponents.Add(TargetActor);
	OnSpotted.Broadcast(TargetActor);
}

void USighterComponent::PerceiveTarget(USightedComponent* TargetActor, bool bWasRemembered)
{
	SpottedSightedComponents.Remove(TargetActor);
	LostSightedComponents.Remove(TargetActor);
	PerceivedSightedComponents.Add(TargetActor);
	OnPerceived.Broadcast(TargetActor, bWasRemembered);
}

void USighterComponent::StartTargetLostWait(USightedComponent* TargetActor)
{
	SpottedSightedComponents.Remove(TargetActor);
	OnLostGracePeriodBegin.Broadcast(TargetActor);
}

void USighterComponent::LoseTarget(USightedComponent* TargetActor, bool bWasActorPerceived)
{
	// 0) sanity
	if (!TargetActor)
	{
		return;
	}

	// 1) Try to remove from the primary list (Perceived).
	const bool bRemovedFromPerceived = PerceivedSightedComponents.Remove(TargetActor) > 0;

	// 2) If it wasn’t perceived, see if it was merely spotted.
	if (!bRemovedFromPerceived && SpottedSightedComponents.Remove(TargetActor) == 0)
	{
		return;
	}

	// 3) Remember actors we once perceived.
	if (bRemovedFromPerceived)
	{
		LostSightedComponents.Add(TargetActor);
	}

	// 4) Notify listeners.
	OnLost.Broadcast(TargetActor, bWasActorPerceived);
}

void USighterComponent::ForgetTarget(USightedComponent* TargetActor)
{
	if (LostSightedComponents.Remove(TargetActor) > 0)
	{
		OnForgotten.Broadcast(TargetActor);
	}
}
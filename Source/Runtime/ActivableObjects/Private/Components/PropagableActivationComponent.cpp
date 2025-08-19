// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/PropagableActivationComponent.h"

#include "Interfaces/ActivableActorInterface.h"

void UPropagableActivationComponent::Activate(bool bReset)
{
	const bool bWasActive = IsActive();
	Super::Activate(bReset);

	if (bWasActive != IsActive())
	{
		PropagateActivation(true);
	}
}

void UPropagableActivationComponent::Deactivate()
{
	const bool bWasActive = IsActive();
	Super::Deactivate();

	if (bWasActive != IsActive())
	{
		PropagateActivation(false);
	}
}

void UPropagableActivationComponent::PropagateActivation(bool bPropagateActivate)
{
	for (auto Itr = LinkedActors.CreateIterator(); Itr; ++Itr)
	{
		UObject* Object = Cast<UObject>(Itr->Target);
		if (!Object) { continue; }
		if (!Object->Implements<UActivableActorInterface>())
		{
			continue;
		}

		const bool bShouldActivate = Itr->bInverse ? !bPropagateActivate : bPropagateActivate;
		if (Itr->bPropagateActivation && bShouldActivate)
		{
			IActivableActorInterface::Execute_Activate(Object);
		}

		if (Itr->bPropagateDeactivation && !bShouldActivate)
		{
			IActivableActorInterface::Execute_Deactivate(Object);
		}
	}
}

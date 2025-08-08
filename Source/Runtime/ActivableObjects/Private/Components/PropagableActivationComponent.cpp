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

void UPropagableActivationComponent::PropagateActivation(const bool bPropagateActivate)
{
	for (auto Itr = LinkedActors.CreateIterator(); Itr; ++Itr)
	{
		if (!Itr) { continue; }
		const UObject* Object = Cast<UObject>(*Itr);
		if (!Object) { continue; }
		if (!Object->Implements<UActivableActorInterface>())
		{
			continue;
		}
		
		if (bPropagateActivate)
		{
			IActivableActorInterface::Execute_Activate(*Itr);
		}
		else
		{
			IActivableActorInterface::Execute_Deactivate(*Itr);
		}
	}
}

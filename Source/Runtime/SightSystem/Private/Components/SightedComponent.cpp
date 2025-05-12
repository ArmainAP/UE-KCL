// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/SightedComponent.h"

#include "Subsystems/SightQueryManager.h"

// Sets default values for this component's properties
USightedComponent::USightedComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
}

void USightedComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (bAutoActivate)
	{
		Activate(true);
	}
}

void USightedComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
	
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	if (USightQueryManager* SightSystem = World->GetSubsystem<USightQueryManager>())
	{
		SightSystem->RegisterSighted(this);
	}
}

void USightedComponent::Deactivate()
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	if (USightQueryManager* SightSystem = World->GetSubsystem<USightQueryManager>())
	{
		SightSystem->UnregisterSighted(this);
	}
	
	Super::Deactivate();
}
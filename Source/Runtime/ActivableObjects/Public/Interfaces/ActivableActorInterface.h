// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/DelayedActivationComponent.h"
#include "UObject/Interface.h"
#include "ActivableActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UActivableActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIVABLEOBJECTS_API IActivableActorInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Activate();
	virtual void Activate_Implementation()
	{
		if (UDelayedActivationComponent* Component = Execute_GetDelayedActivationComponent(_getUObject()))
		{
			Component->TriggerDelayedActivation();
		}
	}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Deactivate();
	virtual void Deactivate_Implementation()
	{
		if (UDelayedActivationComponent* Component = Execute_GetDelayedActivationComponent(_getUObject()))
		{
			Component->TriggerDelayedDeactivation();
		}
	}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UDelayedActivationComponent* GetDelayedActivationComponent() const;
	virtual UDelayedActivationComponent* GetDelayedActivationComponent_Implementation() const
	{
		return nullptr;
	}
};

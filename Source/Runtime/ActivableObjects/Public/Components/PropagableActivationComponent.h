// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/DelayedActivationComponent.h"
#include "PropagableActivationComponent.generated.h"

/**
 * 
 */
UCLASS()
class ACTIVABLEOBJECTS_API UPropagableActivationComponent : public UDelayedActivationComponent
{
	GENERATED_BODY()

public:
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<AActor*> LinkedActors;

protected:
	void PropagateActivation(bool bPropagateActivate);
};

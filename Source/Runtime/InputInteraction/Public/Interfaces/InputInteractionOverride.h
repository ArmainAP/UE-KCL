// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InputInteractionOverride.generated.h"

UINTERFACE(Blueprintable, BlueprintType)
class INPUTINTERACTION_API UInputInteractionOverride : public UInterface
{
	GENERATED_BODY()
};

class INPUTINTERACTION_API IInputInteractionOverride
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	bool ShouldOverrideState(AActor* Interactor, UInputInteractableComponent* InteractingComponent);
	virtual bool ShouldOverrideState_Implementation(AActor* Interactor, UInputInteractableComponent* InteractingComponent) { return false; };

	UFUNCTION(BlueprintNativeEvent)
	bool ShouldOverrideInteraction(AActor* Interactor, UInputInteractableComponent* InteractingComponent);
	virtual bool ShouldOverrideInteraction_Implementation(AActor* Interactor, UInputInteractableComponent* InteractingComponent) { return false; };
};

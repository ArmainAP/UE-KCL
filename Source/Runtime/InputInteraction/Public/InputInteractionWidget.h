// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InputInteractionWidget.generated.h"

class UInputInteractableComponent;

UINTERFACE(Blueprintable)
class INPUTINTERACTION_API UInputInteractionWidget : public UInterface
{
	GENERATED_BODY()
};

class INPUTINTERACTION_API IInputInteractionWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void Initialize(UInputInteractableComponent* OwningComponent);
};

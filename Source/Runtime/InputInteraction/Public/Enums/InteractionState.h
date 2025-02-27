#pragma once

#include "CoreMinimal.h"
#include "InteractionState.generated.h"

UENUM(Blueprintable, BlueprintType)
enum class EInteractionState : uint8
{
	None = 0,
	Obstructed,
	Detected,
	Selected,
};
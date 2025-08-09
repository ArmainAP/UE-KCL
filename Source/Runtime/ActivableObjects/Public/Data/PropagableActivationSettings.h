// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PropagableActivationSettings.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct ACTIVABLEOBJECTS_API FPropagableActivationSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<AActor> Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInverse = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPropagateActivation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPropagateDeactivation = true;
};
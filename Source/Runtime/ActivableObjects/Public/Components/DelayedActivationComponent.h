// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DelayedActivationComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIVABLEOBJECTS_API UDelayedActivationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDelayedActivationComponent();

	UFUNCTION(BlueprintPure)
	bool IsPendingActivation() const;

	UFUNCTION(BlueprintPure)
	bool IsPendingDeactivation() const;
	
	UFUNCTION(BlueprintCallable)
	void TriggerDelayedActivation();

	UFUNCTION(BlueprintCallable)
	void TriggerDelayedDeactivation();

	UFUNCTION(BlueprintCallable)
	void ToggleDelayedActivation();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Delay")
	float ActivationDelay = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Delay")
	float DeactivationDelay = 1.0f;

protected:
	UPROPERTY()
	FTimerHandle TimerHandle;
};

// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputInteractableComponent.h"
#include "Components/ActorComponent.h"
#include "Components/SphereTraceMultiComponent.h"
#include "InputInteractorComponent.generated.h"

class UInputAction;
class UInputInteractableComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerInteract, UInputInteractableComponent*, Interactable);

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class INPUTINTERACTION_API UInputInteractorComponent : public USphereTraceMultiComponent
{
	GENERATED_BODY()

public:
	UInputInteractorComponent();
	virtual void SetActive(bool bNewActive, bool bReset) override;
	virtual void Deactivate() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void InteractionInput(const FGameplayTag GameplayTag, const bool bPressed = false);
	
protected:
	void TraceInteractables();
	bool CheckInteractable(UInputInteractableComponent* InputInteractableComponent) const;
	void UpdateSelectedInteractable();
	UInputInteractableComponent* SelectInteractable();
	bool ExecuteObstructionTrace(const UInputInteractableComponent* Interactable) const;

	void UpdateSelectedInput();
	void Interact(UInputInteractableComponent* Interactable);
	bool SetCurrentInteractionTime(float NewInteractionTime, const UInputInteractableComponent* Interactable);

	void UpdateInteractableState(UInputInteractableComponent* Interactable, const EInteractionState NewState) const;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	bool DetectCameraProximity = true;

	UPROPERTY(BlueprintAssignable, Category = "Interact")
	FOnPlayerInteract OnInteract;
	
protected:
	UPROPERTY()
	TSet<UInputInteractableComponent*> DetectedInteractables;

	UPROPERTY()
	UInputInteractableComponent* SelectedInteractable;

	UPROPERTY()
	TSet<FGameplayTag> ActiveKeys = {};

	float CachedDeltatime = 0.f;
	float CurrentInteractionTime = 0.f;
	float TimeSinceInteraction = 0.f;
};
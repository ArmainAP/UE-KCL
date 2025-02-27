// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InputInteractableComponent.generated.h"

class UInputInteractableDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStateUpdated, UInputInteractableComponent*, Interactable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionPercentageUpdated, float, Percentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteract, AActor*, Interactor, AActor*, Interactable);

UENUM(Blueprintable, BlueprintType)
enum class EInteractionState : uint8
{
	None = 0,
	Obstructed,
	Detected,
	Selected,
};

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class INPUTINTERACTION_API UInputInteractableComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UInputInteractableComponent();
	virtual void BeginPlay() override;
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;

	UFUNCTION(BlueprintCallable)
	UInputInteractableDataAsset* GetInteractableDataAsset() const;

	UFUNCTION(BlueprintCallable)
	void UpdateState(EInteractionState NewState);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsObstructed() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsDetected() const;

	UPROPERTY(BlueprintAssignable, Category = "Interactable")
	FStateUpdated OnStateUpdate;

	UPROPERTY(BlueprintAssignable, Category = "Interactable")
	FInteractionPercentageUpdated OnInteractionPercentageUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Interactable")
    FInteract OnInteract;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UInputInteractableDataAsset* InteractableData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EInteractionState InteractionState = EInteractionState::None;
};
// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputInteractableComponent.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InputInteractorComponent.generated.h"

class UInputAction;
class UInputInteractableComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerInteract, UInputInteractableComponent*, Interactable);

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class INPUTINTERACTION_API UInputInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInputInteractorComponent();
	virtual void SetActive(bool bNewActive, bool bReset) override;
	virtual void Deactivate() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION()
	FORCEINLINE EDrawDebugTrace::Type GetDrawDebugType() const { return bDebugTraces ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None; }

	UFUNCTION(BlueprintCallable, Category = "Input")
	void InteractionInput(UInputAction* Key, bool bPressed = false);
	
protected:
	bool TraceInteractables();
	bool CheckInteractable(UInputInteractableComponent* InputInteractableComponent) const;
	void UpdateSelectedInteractable();
	UInputInteractableComponent* SelectInteractable();
	bool ExecuteObstructionTrace(const UInputInteractableComponent* Interactable) const;

	void UpdateSelectedInput();
	void Interact(UInputInteractableComponent* Interactable);
	bool SetCurrentInteractionTime(float NewInteractionTime, const UInputInteractableComponent* Interactable);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float DetectionRadius = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	bool bTraceComplex = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDebugTraces = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float DebugTraceDuration = 0.1f;

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
	TSet<UInputAction*> ActiveKeys = {};

	float CachedDeltatime = 0.f;
	float CurrentInteractionTime = 0.f;
	float TimeSinceInteraction = 0.f;
};
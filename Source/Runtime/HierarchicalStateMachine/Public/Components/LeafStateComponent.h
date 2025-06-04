// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "LeafStateComponent.generated.h"

class UStateMachineComponent;

UCLASS( ClassGroup=(StateMachines), meta=(BlueprintSpawnableComponent) )
class HIERARCHICALSTATEMACHINE_API ULeafStateComponent : public UActorComponent
{
	GENERATED_BODY()

friend UStateMachineComponent;
	
public:
	// Sets default values for this component's properties
	ULeafStateComponent();

	UFUNCTION(BlueprintCallable)
	void RegisterTransition(const FGameplayTag& To, int Index = -1);
	
	UFUNCTION(BlueprintCallable)
	FGameplayTag GetTransitionTag(const FGameplayTag& FromTag) const;

	UFUNCTION(BlueprintNativeEvent)
	bool CanTransition(const FGameplayTag& FromTag, const FGameplayTag& ToTag) const;

protected:
	UFUNCTION(BlueprintGetter)
	UStateMachineComponent* GetStateMachine() const { return StateMachine; };
	
	UFUNCTION(BlueprintSetter)
	void SetStateMachine(UStateMachineComponent* InStateMachine) { StateMachine = InStateMachine; };
	
	UFUNCTION()
	void HandleComponentActivated(UActorComponent* Component, bool bReset);

	UFUNCTION(BlueprintNativeEvent)
	void StateEnter();
	virtual void StateEnter_Implementation() {};
	
	UFUNCTION()
	void HandleComponentDeactivated(UActorComponent* Component);

	UFUNCTION(BlueprintNativeEvent)
	void StateExit();
	virtual void StateExit_Implementation() {};

private:
	UPROPERTY(BlueprintGetter=GetStateMachine, BlueprintSetter=SetStateMachine)
	TObjectPtr<UStateMachineComponent> StateMachine;

	UPROPERTY(EditAnywhere)
	TArray<FGameplayTag> AutoTransitions;
};

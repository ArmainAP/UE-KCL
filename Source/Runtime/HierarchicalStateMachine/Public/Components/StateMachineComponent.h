// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LeafStateComponent.h"
#include "StateMachineComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStateChanged, FGameplayTag, FromState, FGameplayTag, ToState);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(StateMachine), meta=(BlueprintSpawnableComponent))
class HIERARCHICALSTATEMACHINE_API UStateMachineComponent : public ULeafStateComponent
{
	GENERATED_BODY()

public:
	UStateMachineComponent() = default;
	
	UFUNCTION(BlueprintCallable)
	void RegisterStateComponent(const FGameplayTag& StateTag, ULeafStateComponent* Component);

	UFUNCTION(BlueprintCallable)
	void EnterState(const FGameplayTag& Tag, bool bForce = false);

	UFUNCTION(BlueprintCallable)
	void ExitState(const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable)
	void PopState();

	UFUNCTION(BlueprintPure)
	bool IsStateInStack(const FGameplayTag& Tag) const;

	UFUNCTION(BlueprintPure)
	int32 GetStateDepth() const;

	UFUNCTION(BlueprintGetter)
	FGameplayTag GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintGetter)
	int32 GetMaxStateDepth() const { return MaxStateDepth; }

	UFUNCTION(BlueprintSetter)
	void SetMaxStateDepth(const int32 NewValue) { MaxStateDepth = NewValue; }

	UFUNCTION(BlueprintCallable)
	void RegisterTransition(const FGameplayTag& From, const FGameplayTag& To);

	UPROPERTY(BlueprintAssignable)
	FOnStateChanged OnStateChanged;

protected:
	virtual void StateEnter_Implementation() override;
	virtual void StateExit_Implementation() override;

	UFUNCTION()
	void HandleStateDeactivated(UActorComponent* Component);
	
	UPROPERTY(VisibleAnywhere, BlueprintGetter=GetCurrentState)
	FGameplayTag CurrentState;

	UPROPERTY(VisibleAnywhere)
	TMap<FGameplayTag, TWeakObjectPtr<ULeafStateComponent>> StateComponents;

	UPROPERTY(VisibleAnywhere)
	TMap<TWeakObjectPtr<ULeafStateComponent>, FGameplayTag> ReverseStateComponents;

	UPROPERTY(VisibleAnywhere)
	TArray<FGameplayTag> StateStack;

	UPROPERTY(EditAnywhere, BlueprintGetter=GetMaxStateDepth, BlueprintSetter=SetMaxStateDepth)
	int32 MaxStateDepth = 10;

	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FGameplayTag> AutoTransitions;
};

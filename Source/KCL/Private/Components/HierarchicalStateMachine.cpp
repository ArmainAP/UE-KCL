// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/HierarchicalStateMachine.h"
#include "Logging.h"

UHierarchicalStateMachine::UHierarchicalStateMachine()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	OnComponentActivated.AddUniqueDynamic(this, &UHierarchicalStateMachine::HandleOnComponentActivated);
	OnComponentDeactivated.AddUniqueDynamic(this, &UHierarchicalStateMachine::HandleOnComponentDeactivated);
}

void UHierarchicalStateMachine::RegisterStateComponent(const FGameplayTag& StateTag, UActorComponent* StateComponent)
{
	if (!StateTag.IsValid())
	{
		UE_LOG(LogKCL, Error, TEXT("%s >> Tried to register component with invalid tag"), StringCast<TCHAR>(__FUNCTION__).Get());
		return;
	}

	if (!StateComponent)
	{
		UE_LOG(LogKCL, Error, TEXT("%s >> Tried to register invalid component"), StringCast<TCHAR>(__FUNCTION__).Get());
		return;
	}

	StateComponents.Add(StateTag, StateComponent);
}

void UHierarchicalStateMachine::EnterState(const FGameplayTag& Tag, bool bForce)
{
	if (CurrentState == Tag && !bForce)
	{
		return;
	}

	TWeakObjectPtr<UActorComponent>* ComponentPtr = StateComponents.Find(Tag);
	if (!ComponentPtr || !ComponentPtr->IsValid())
	{
		UE_LOG(LogKCL, Warning, TEXT("%s >> Invalid or missing component for tag: %s"), StringCast<TCHAR>(__FUNCTION__).Get(), *Tag.ToString());
		return;
	}

	if (CurrentState.IsValid())
	{
		StateStack.Push(CurrentState);
		if (StateStack.Num() > MaxStateDepth)
		{
			UE_LOG(LogKCL, Warning, TEXT("State stack exceeded max depth of %d"), MaxStateDepth);
			StateStack.Pop();
			return;
		}
	}

	FGameplayTag PreviousState = CurrentState;
	CurrentState = Tag;

	if (UActorComponent* Component = ComponentPtr->Get())
	{
		Component->Activate(bForce);
	}

	OnStateChanged.Broadcast(PreviousState, CurrentState);
}

void UHierarchicalStateMachine::ExitState(const FGameplayTag& Tag)
{
	TWeakObjectPtr<UActorComponent>* ComponentPtr = StateComponents.Find(Tag);
	if (!ComponentPtr || !ComponentPtr->IsValid())
	{
		return;
	}

	if (UActorComponent* Component = ComponentPtr->Get())
	{
		Component->Deactivate();
	}

	PopState();
}

void UHierarchicalStateMachine::PopState()
{
	if (StateStack.IsEmpty())
	{
		CurrentState = FGameplayTag();
		return;
	}

	const FGameplayTag NewState = StateStack.Pop();
	const FGameplayTag Prev     = CurrentState;
	CurrentState                = NewState;

	// Reactivate the component without altering the stack again
	if (TWeakObjectPtr<UActorComponent>* CompPtr = StateComponents.Find(NewState))
	{
		if (UActorComponent* Comp = CompPtr->Get())
		{
			Comp->Activate(true);
		}
	}
	OnStateChanged.Broadcast(Prev, NewState);
}

bool UHierarchicalStateMachine::IsStateInStack(const FGameplayTag& Tag) const
{
	return StateStack.Contains(Tag);
}

int32 UHierarchicalStateMachine::GetStateDepth() const
{
	return StateStack.Num();
}

void UHierarchicalStateMachine::HandleOnComponentActivated(UActorComponent* Component, bool bReset)
{
	EnterState(CurrentState, bReset);
}

void UHierarchicalStateMachine::HandleOnComponentDeactivated(UActorComponent* Component)
{
	ExitState(CurrentState);
}
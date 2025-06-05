// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/StateMachineComponent.h"
#include "Logging.h"

void UStateMachineComponent::RegisterStateComponent(const FGameplayTag& StateTag, ULeafStateComponent* StateComponent)
{
	if (!StateTag.IsValid())
	{
		UE_LOG(LogHierarchicalStateMachine, Error, TEXT("%s >> Tried to register component with invalid tag"), StringCast<TCHAR>(__FUNCTION__).Get());
		return;
	}

	if (!StateComponent)
	{
		UE_LOG(LogHierarchicalStateMachine, Error, TEXT("%s >> Tried to register invalid component"), StringCast<TCHAR>(__FUNCTION__).Get());
		return;
	}

	StateComponents.Add(StateTag, StateComponent);
	ReverseStateComponents.Add(StateComponent, StateTag);
	StateComponent->OnComponentDeactivated.AddUniqueDynamic(this, &UStateMachineComponent::HandleStateDeactivated);
	StateComponent->SetStateMachine(this);
}

void UStateMachineComponent::EnterState(const FGameplayTag& Tag, bool bForce)
{
	if (CurrentState == Tag && !bForce)
	{
		return;
	}

	const TWeakObjectPtr<ULeafStateComponent>* ComponentPtr = StateComponents.Find(Tag);
	if (!ComponentPtr || !ComponentPtr->IsValid())
	{
		UE_LOG(LogHierarchicalStateMachine, Warning, TEXT("%s >> Invalid or missing component for tag: %s"), StringCast<TCHAR>(__FUNCTION__).Get(), *Tag.ToString());
		return;
	}

	if (CurrentState.IsValid())
	{
		StateStack.Push(CurrentState);
		if (StateStack.Num() > MaxStateDepth)
		{
			UE_LOG(LogHierarchicalStateMachine, Warning, TEXT("State stack exceeded max depth of %d"), MaxStateDepth);
			StateStack.Pop();
			return;
		}
	}

	const FGameplayTag PreviousState = CurrentState;
	CurrentState = Tag;

	if (UActorComponent* Component = ComponentPtr->Get())
	{
		Component->Activate(bForce);
	}

	OnStateChanged.Broadcast(PreviousState, CurrentState);
}

void UStateMachineComponent::ExitState(const FGameplayTag& Tag)
{
	const TWeakObjectPtr<ULeafStateComponent>* ComponentPtr = StateComponents.Find(Tag);
	if (!ComponentPtr || !ComponentPtr->IsValid())
	{
		return;
	}

	if (ULeafStateComponent* Component = ComponentPtr->Get())
	{
		Component->Deactivate();
	}

	PopState();
}

void UStateMachineComponent::PopState()
{
	if (StateStack.IsEmpty())
	{
		CurrentState = FGameplayTag();
		return;
	}

	const FGameplayTag NewState = StateStack.Pop();
	const FGameplayTag Prev = CurrentState;
	CurrentState = NewState;

	// Reactivate the component without altering the stack again
	if (TWeakObjectPtr<ULeafStateComponent>* CompPtr = StateComponents.Find(NewState))
	{
		if (UActorComponent* Comp = CompPtr->Get())
		{
			Comp->Activate(true);
		}
	}
	OnStateChanged.Broadcast(Prev, NewState);
}

bool UStateMachineComponent::IsStateInStack(const FGameplayTag& Tag) const
{
	return StateStack.Contains(Tag);
}

int32 UStateMachineComponent::GetStateDepth() const
{
	return StateStack.Num();
}

void UStateMachineComponent::StateEnter_Implementation()
{
	Super::StateEnter_Implementation();
	EnterState(CurrentState);
}

void UStateMachineComponent::StateExit_Implementation()
{
	Super::StateExit_Implementation();
	ExitState(CurrentState);
}

void UStateMachineComponent::HandleStateDeactivated(UActorComponent* Component)
{
	ULeafStateComponent* LeafStateComponent = Cast<ULeafStateComponent>(Component);
	const FGameplayTag* FromTag = ReverseStateComponents.Find(LeafStateComponent);
	if (!FromTag) { return; }

	const FGameplayTag& ToTag = GetTransitionTag(*FromTag, LeafStateComponent);
	if (ToTag.IsValid())
	{
		EnterState(ToTag);
	}
	else
	{
		PopState();
	}
}

FGameplayTag UStateMachineComponent::GetTransitionTag(const FGameplayTag& FromTag, ULeafStateComponent* LeafStateComponent) const
{
	if (!LeafStateComponent) { return {}; }
	for (const FGameplayTag& TransitionTag : LeafStateComponent->AutoTransitions)
	{
		const TWeakObjectPtr<ULeafStateComponent>* TransitionComponent = StateComponents.Find(TransitionTag);
		if (!TransitionComponent) { continue; }
		
		if (TransitionComponent->Get()->CanTransition(FromTag, TransitionTag))
		{
			return TransitionTag;
		}
	}
	return {};
}


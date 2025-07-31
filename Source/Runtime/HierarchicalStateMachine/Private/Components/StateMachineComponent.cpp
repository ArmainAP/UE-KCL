// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/StateMachineComponent.h"
#include "Logging.h"

bool UStateMachineComponent::RegisterStateComponent(const FGameplayTag& StateTag, ULeafStateComponent* StateComponent)
{
	if (!StateTag.IsValid())
	{
		UE_LOG(LogHierarchicalStateMachine, Error, TEXT("%s >> Tried to register component with invalid tag"), StringCast<TCHAR>(__FUNCTION__).Get());
		return false;
	}

	if (!StateComponent)
	{
		UE_LOG(LogHierarchicalStateMachine, Error, TEXT("%s >> Tried to register invalid component"), StringCast<TCHAR>(__FUNCTION__).Get());
		return false;
	}

	if (StateComponents.Contains(StateTag))
	{
		UE_LOG(LogHierarchicalStateMachine, Warning, TEXT("%s >> State %s already registered; ignoring duplicate"), *FString(__FUNCTION__), *StateTag.ToString());
		return false;
	}

	StateComponents.Add(StateTag, StateComponent);
	ReverseStateComponents.Add(StateComponent, StateTag);
	StateComponent->SetStateMachine(this);
	return true;
}

bool UStateMachineComponent::EnterState(const FGameplayTag& Tag)
{
	if (!IsActive())
	{
		return false;
	}
	
	ULeafStateComponent* Component = GetStateComponent(Tag);
	if (!Component)
	{
		return false;
	}

	if (!Component->CanEnter(CurrentState, Tag))
	{
		return false;
	}

	ULeafStateComponent* CurrentStateComponent = GetCurrentStateComponent();
	if (CurrentStateComponent)
	{
		if (!CurrentStateComponent->CanExit(CurrentState, Tag))
		{
			return false;
		}
	
		CurrentStateComponent->RequestExit(EStateExitReason::Canceled);
	}

	PushStateToStack(CurrentState);

	const FGameplayTag PrevState = CurrentState;
	CurrentState = Tag;

	LogState(StringCast<TCHAR>(__FUNCTION__).Get(), CurrentState.ToString());

	Component->OnComponentDeactivated.AddUniqueDynamic(this, &UStateMachineComponent::HandleStateDeactivated);
	Component->Activate(true);
	OnStateChanged.Broadcast(PrevState, CurrentState);
	return true;
}

bool UStateMachineComponent::ExitState(const FGameplayTag& Tag, const bool bForce) const
{
	ULeafStateComponent* Component = GetStateComponent(Tag);
	if (!Component)
	{
		return true;
	}
	
	if (bForce || Component->CanExit(CurrentState, Tag))
	{
		Component->OnComponentDeactivated.RemoveDynamic(this, &UStateMachineComponent::HandleStateDeactivated);
		Component->RequestExit(EStateExitReason::Aborted);
		LogState(StringCast<TCHAR>(__FUNCTION__).Get(), Tag.ToString());
	}

	return !Component->IsActive();
}

bool UStateMachineComponent::PopState()
{
	if (!IsActive())
	{
		return false;
	}
	
	if (StateStack.IsEmpty())
	{
		CurrentState = FGameplayTag();
		return false;
	}

	if (!ExitState(CurrentState))
	{
		return false;
	}
	
	const FGameplayTag Prev = CurrentState;
	const FGameplayTag Popped = StateStack.Pop();
	LogState(StringCast<TCHAR>(__FUNCTION__).Get(), Popped.ToString());

	if (EnterState(Popped))
	{
		OnStateChanged.Broadcast(Prev, CurrentState);
		return true;
	}

	return PopState();
}

bool UStateMachineComponent::IsStateInStack(const FGameplayTag& Tag) const
{
	return StateStack.Contains(Tag);
}

int32 UStateMachineComponent::GetStateDepth() const
{
	return StateStack.Num();
}

ULeafStateComponent* UStateMachineComponent::GetStateComponent(const FGameplayTag& Tag) const
{
	const TWeakObjectPtr<ULeafStateComponent>* CompPtr = StateComponents.Find(Tag);
	return CompPtr ? CompPtr->Get() : nullptr;
}

FGameplayTag UStateMachineComponent::GetStateByComponent(const ULeafStateComponent* LeafStateComponent)
{
	const FGameplayTag* Tag = ReverseStateComponents.Find(LeafStateComponent);
	return Tag ? *Tag : FGameplayTag();
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
	if (!LeafStateComponent)
	{
		return;
	}

	const FGameplayTag& FromTag = GetStateByComponent(LeafStateComponent);
	if (!FromTag.IsValid())
	{
		return;
	}

	if (LeafStateComponent->ExitReason != EStateExitReason::Completed)
	{
		LogState(StringCast<TCHAR>(__FUNCTION__).Get(), FromTag.ToString());
		return;
	}

	const FGameplayTag& ToTag = TransitionState(FromTag, LeafStateComponent);
	if (ToTag.IsValid())
	{
		EnterState(ToTag);
	}
	else
	{
		PopState();
	}
}

FGameplayTag UStateMachineComponent::TransitionState(const FGameplayTag& FromTag, ULeafStateComponent* CurrentLeaf) const
{
	if (!CurrentLeaf)
	{
		return {};
	}
	
	for (const FGameplayTag& CandidateTag : CurrentLeaf->AutoTransitions)
	{
		if (!CurrentLeaf->CanExit(FromTag, CandidateTag))
		{
			continue;
		}
		
		const ULeafStateComponent* CandidateLeaf = GetStateComponent(CandidateTag);
		if (!CandidateLeaf)
		{
			continue;
		}

		if (CandidateLeaf->CanEnter(FromTag, CandidateTag))
		{
			LogState(StringCast<TCHAR>(__FUNCTION__).Get(), CandidateTag.ToString());
			return CandidateTag;
		}
	}
	
	return {};
}

bool UStateMachineComponent::PushStateToStack(const FGameplayTag& State)
{
	if (!State.IsValid())
	{
		return false;
	}
	
	if (StateStack.Num() >= MaxStateDepth)
	{
		UE_LOG(LogHierarchicalStateMachine, Warning, TEXT("State stack exceeded max depth of %d"), MaxStateDepth);
		return false;
	}

	StateStack.Push(CurrentState);
	LogState(StringCast<TCHAR>(__FUNCTION__).Get(), CurrentState.ToString());
	return true;
}

void UStateMachineComponent::LogState(const FString& Function, const FString& State) const
{
	UE_LOG(LogHierarchicalStateMachine, Log, TEXT("%s >> %s >> %s >> %s"), *GetOwner()->GetName(), *GetName(), *Function, *State);
}

void UStateMachineComponent::ClearStateStack(const bool bForce)
{
	ExitState(CurrentState, bForce);

	while (!StateStack.IsEmpty())
	{
		ExitState(StateStack.Pop());
	}
}
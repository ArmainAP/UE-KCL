// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/StateComponents/MoveTaskStateComponent.h"

#include "KiraHelperLibrary.h"

void UMoveTaskStateComponent::StateEnter_Implementation()
{
	Super::StateEnter_Implementation();

	AutoRestartCounter = 0;

	const APawn* OwnerPawn = UKiraHelperLibrary::GetPawn(GetOwner());
	check(OwnerPawn);
	OwnerTaskMoveToComponent = OwnerPawn->FindComponentByClass<UTaskMoveToComponent>();
	
	if (bBindEventsOnEnter)
	{
		BindMoveEvents();
	}
}

void UMoveTaskStateComponent::StateExit_Implementation()
{
	if (bUnbindEventsOnExit)
	{
		UnbindMoveEvents();
	}
	
	Super::StateExit_Implementation();
}

void UMoveTaskStateComponent::OnMoveStarted(UTaskMoveToComponent* TaskMoveToComponent, int ContextID)
{
	if (ContextID != INDEX_NONE)
	{
		MoveTaskID = ContextID;
		HandleMoveStarted(TaskMoveToComponent, ContextID);
	}
}

void UMoveTaskStateComponent::OnContextInvalidated(UTaskMoveToComponent* TaskMoveToComponent, int ContextID)
{
	if (MoveTaskID != INDEX_NONE && MoveTaskID == ContextID)
	{
		HandleContextInvalidated(TaskMoveToComponent, ContextID);
	}
}

void UMoveTaskStateComponent::OnContextFailed(UTaskMoveToComponent* TaskMoveToComponent, int ContextID)
{
	if (MoveTaskID != INDEX_NONE && MoveTaskID == ContextID)
	{
		HandleContextFailed(TaskMoveToComponent, ContextID);
	}
}

void UMoveTaskStateComponent::OnContextFinished(UTaskMoveToComponent* TaskMoveToComponent, int ContextID)
{
	if (MoveTaskID != INDEX_NONE && MoveTaskID == ContextID)
	{
		HandleContextFinished(TaskMoveToComponent, ContextID);
	}
}

void UMoveTaskStateComponent::OnMoveStopped(UTaskMoveToComponent* TaskMoveToComponent, int ContextID)
{
	if (MoveTaskID != INDEX_NONE && MoveTaskID == ContextID)
	{
		HandleMoveStopped(TaskMoveToComponent, ContextID);
	}
}

void UMoveTaskStateComponent::HandleContextInvalidated_Implementation(UTaskMoveToComponent* TaskMoveToComponent, int ContextID)
{
	if (bRestartMoveOnInvalidation && CanAutoRestart())
	{
		AutoRestartCounter++;
		ExecuteMoveTask();
	}
}

void UMoveTaskStateComponent::HandleContextFailed_Implementation(UTaskMoveToComponent* TaskMoveToComponent, int ContextID)
{
	if (bRestartMoveOnFailure && CanAutoRestart())
	{
		AutoRestartCounter++;
		ExecuteMoveTask();
	}
}

void UMoveTaskStateComponent::HandleMoveStopped_Implementation(UTaskMoveToComponent* TaskMoveToComponent, int ContextID)
{
	if (bRestartMoveOnStop && CanAutoRestart())
	{
		AutoRestartCounter++;
		ExecuteMoveTask();
	}
}

void UMoveTaskStateComponent::BindMoveEvents()
{
	if (OwnerTaskMoveToComponent.IsValid())
	{
		OwnerTaskMoveToComponent->OnMoveStarted.AddUniqueDynamic(this, &UMoveTaskStateComponent::OnMoveStarted);
		OwnerTaskMoveToComponent->OnContextInvalidated.AddUniqueDynamic(this, &UMoveTaskStateComponent::OnContextInvalidated);
		OwnerTaskMoveToComponent->OnContextFailed.AddUniqueDynamic(this, &UMoveTaskStateComponent::OnContextFailed);
		OwnerTaskMoveToComponent->OnContextFinished.AddUniqueDynamic(this, &UMoveTaskStateComponent::OnContextFinished);
		OwnerTaskMoveToComponent->OnMoveStopped.AddUniqueDynamic(this, &UMoveTaskStateComponent::OnMoveStopped);
	}
}

void UMoveTaskStateComponent::UnbindMoveEvents()
{
	if (OwnerTaskMoveToComponent.IsValid())
	{
		OwnerTaskMoveToComponent->OnMoveStarted.RemoveDynamic(this, &UMoveTaskStateComponent::OnMoveStarted);
		OwnerTaskMoveToComponent->OnContextInvalidated.RemoveDynamic(this, &UMoveTaskStateComponent::OnContextInvalidated);
		OwnerTaskMoveToComponent->OnContextFailed.RemoveDynamic(this, &UMoveTaskStateComponent::OnContextFailed);
		OwnerTaskMoveToComponent->OnContextFinished.RemoveDynamic(this, &UMoveTaskStateComponent::OnContextFinished);
		OwnerTaskMoveToComponent->OnMoveStopped.RemoveDynamic(this, &UMoveTaskStateComponent::OnMoveStopped);
	}
}

bool UMoveTaskStateComponent::CanAutoRestart() const
{
	switch (AutoRestartCounter)
	{
	case INDEX_NONE: return true;
	case 0: return false;
	default: return AutoRestartCounter < AutoRestartLimit;
	}
}

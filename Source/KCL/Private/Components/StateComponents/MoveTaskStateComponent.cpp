// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/StateComponents/MoveTaskStateComponent.h"

#include "KiraHelperLibrary.h"

void UMoveTaskStateComponent::StateEnter_Implementation()
{
	Super::StateEnter_Implementation();

	APawn* OwnerPawn = UKiraHelperLibrary::GetPawn(GetOwner());
	check(OwnerPawn);

	OwnerTaskMoveToComponent = OwnerPawn->FindComponentByClass<UTaskMoveToComponent>();
	if (OwnerTaskMoveToComponent.IsValid())
	{
		OwnerTaskMoveToComponent->OnMoveStarted.AddUniqueDynamic(this, &UMoveTaskStateComponent::OnMoveStarted);
		OwnerTaskMoveToComponent->OnContextInvalidated.AddUniqueDynamic(this, &UMoveTaskStateComponent::OnContextInvalidated);
		OwnerTaskMoveToComponent->OnContextFailed.AddUniqueDynamic(this, &UMoveTaskStateComponent::OnContextFailed);
		OwnerTaskMoveToComponent->OnContextFinished.AddUniqueDynamic(this, &UMoveTaskStateComponent::OnContextFinished);
		OwnerTaskMoveToComponent->OnMoveStopped.AddUniqueDynamic(this, &UMoveTaskStateComponent::OnMoveStopped);
	}
}

void UMoveTaskStateComponent::StateExit_Implementation()
{
	if (OwnerTaskMoveToComponent.IsValid())
	{
		OwnerTaskMoveToComponent->OnMoveStarted.RemoveDynamic(this, &UMoveTaskStateComponent::OnMoveStarted);
		OwnerTaskMoveToComponent->OnContextInvalidated.RemoveDynamic(this, &UMoveTaskStateComponent::OnContextInvalidated);
		OwnerTaskMoveToComponent->OnContextFailed.RemoveDynamic(this, &UMoveTaskStateComponent::OnContextFailed);
		OwnerTaskMoveToComponent->OnContextFinished.RemoveDynamic(this, &UMoveTaskStateComponent::OnContextFinished);
		OwnerTaskMoveToComponent->OnMoveStopped.RemoveDynamic(this, &UMoveTaskStateComponent::OnMoveStopped);
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

// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/InputInteractableComponent.h"

#include "InputInteractionWidget.h"

UInputInteractableComponent::UInputInteractableComponent()
{
	bAutoActivate = true;
    SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawAtDesiredSize(true);
}

void UInputInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	SetActive(bAutoActivate, true);
}

void UInputInteractableComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	UUserWidget* UserWidget = GetWidget();
	if (!UserWidget)
	{
		return;
	}
	
	if (UserWidget->Implements<UInputInteractionWidget>())
	{
		IInputInteractionWidget::Execute_Initialize(UserWidget, this);
	}

	UserWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UInputInteractableComponent::Deactivate()
{
	Super::Deactivate();

	InteractionState = EInteractionState::None;
	
	UUserWidget* UserWidget = GetWidget();
	if (!UserWidget)
	{
		return;
	}

	UserWidget->SetVisibility(ESlateVisibility::Hidden);
	UpdateState(EInteractionState::None);
}

UInputInteractableDataAsset* UInputInteractableComponent::GetInteractableDataAsset() const
{
	return InteractableData;
}

void UInputInteractableComponent::UpdateState(const EInteractionState NewState)
{
	if (InteractionState == NewState)
	{
		return;
	}
	InteractionState = NewState;
	OnStateUpdate.Broadcast(this);
}

bool UInputInteractableComponent::IsObstructed() const
{
	return InteractionState == EInteractionState::Obstructed;
}

bool UInputInteractableComponent::IsDetected() const
{
	return InteractionState > EInteractionState::Obstructed;
}

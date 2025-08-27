// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/InputInteractorComponent.h"

#include "Components/InputInteractableComponent.h"
#include "Data/InputInteractionDataAsset.h"
#include "Interfaces/InputInteractionOverride.h"
#include "Kismet/GameplayStatics.h"

UInputInteractorComponent::UInputInteractorComponent()
{
	bAutoActivate = true;
	Radius = 1000;
	TraceChannel = ECC_Camera;
}

void UInputInteractorComponent::SetActive(const bool bNewActive, const bool bReset)
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	// No need to activate if owner is not a pawn, and it is not locally controlled. 
	if (!IsValid(Pawn))
	{
		Deactivate();
		return;
	}

	// No need to activate if owner is not locally controlled. 
	if (!Pawn->IsLocallyControlled())
	{
		Deactivate();
		return;
	}

	// No need to activate if the owner is not controlled by a PlayerController.
	if (!Cast<APlayerController>(Pawn->GetController()))
	{
		Deactivate();
	}

	ActorsToIgnore.AddUnique(Pawn);
	
	Super::SetActive(bNewActive, bReset);
}

void UInputInteractorComponent::Deactivate()
{
	Super::Deactivate();

	for (UInputInteractableComponent* InputInteractableComponent : DetectedInteractables)
	{
		UpdateInteractableState(InputInteractableComponent, EInteractionState::None);
		SetCurrentInteractionTime(0.f, InputInteractableComponent);
	}
	DetectedInteractables.Empty();
	SelectedInteractable = nullptr;
}

void UInputInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CachedDeltatime = DeltaTime;
	TimeSinceInteraction += DeltaTime;
	UpdateSelectedInteractable();
	UpdateSelectedInput();
}

void UInputInteractorComponent::InteractionInput(const FGameplayTag GameplayTag, const bool bPressed)
{
	if (!GameplayTag.IsValid())
	{
		return;
	}
	
	if (bPressed)
	{
		ActiveKeys.Add(GameplayTag);
		Interact(SelectedInteractable);
	}
	else
	{
		ActiveKeys.Remove(GameplayTag);
		SetCurrentInteractionTime(0.f, SelectedInteractable);
	}
}

void UInputInteractorComponent::TraceInteractables()
{
	for (const FHitResult& HitResult : CachedHitResults)
	{
		const AActor* Actor = HitResult.GetActor();
		if (!IsValid(Actor))
		{
			continue;
		}

		TArray<UInputInteractableComponent*> Components;
		Actor->GetComponents<UInputInteractableComponent>(Components);
		DetectedInteractables.Append(Components);
	}
}

bool UInputInteractorComponent::CheckInteractable(UInputInteractableComponent* InputInteractableComponent) const
{
	if (!IsValid(InputInteractableComponent))
	{
		return false;
	}

	if (!InputInteractableComponent->IsActive())
	{
		UpdateInteractableState(InputInteractableComponent, EInteractionState::None);
		return false;
	}

	const UInputInteractableDataAsset* Data = InputInteractableComponent->GetInteractableDataAsset();
	if (!IsValid(Data))
	{
		return false;
	}
		
	const float DistanceFromOrigin = FVector::Distance(GetOwner()->GetActorLocation(), InputInteractableComponent->GetComponentLocation());
	const bool bDetected = DistanceFromOrigin < Data->MaxDetectionDistance;
	if (!bDetected)
	{
		UpdateInteractableState(InputInteractableComponent, EInteractionState::None);
		return false;
	}
	
	if (ExecuteObstructionTrace(InputInteractableComponent))
	{
		UpdateInteractableState(InputInteractableComponent, EInteractionState::Obstructed);
		return false;
	}

	UpdateInteractableState(InputInteractableComponent, EInteractionState::Detected);
	return true;
}

void UInputInteractorComponent::UpdateSelectedInteractable()
{
	TraceInteractables();

	UInputInteractableComponent* UpdatedInteractable = SelectInteractable();
	if (UpdatedInteractable != SelectedInteractable)
	{
		// Reset state for old interactable
		CheckInteractable(SelectedInteractable);
		SetCurrentInteractionTime(0.f, SelectedInteractable);
		SetCurrentInteractionTime(0.f, UpdatedInteractable);
	}
	
	SelectedInteractable = UpdatedInteractable;
	UpdateInteractableState(SelectedInteractable, EInteractionState::Selected);
}

UInputInteractableComponent* UInputInteractorComponent::SelectInteractable()
{
	if (DetectedInteractables.IsEmpty())
	{
		return nullptr;
	}
	
	float TargetDistanceFromOrigin = Radius;
	const FVector CharacterLocation = GetOwner()->GetActorLocation();

	FVector CameraLocation;
	FRotator CameraRotation;
	GetOwner()->GetActorEyesViewPoint(CameraLocation, CameraRotation);
	const FVector CharacterDirection = CameraRotation.Vector();

	UInputInteractableComponent* NewSelectedComponent = nullptr;
	for (UInputInteractableComponent* Interactable : DetectedInteractables.Array())
	{
		if (!CheckInteractable(Interactable))
		{
			DetectedInteractables.Remove(Interactable);
			continue;
		}

		const UInputInteractableDataAsset* Data = Interactable->GetInteractableDataAsset();
		if (!IsValid(Data))
		{
			DetectedInteractables.Remove(Interactable);
			continue;
		}

		const float DistanceFromCharacter = FVector3d::Distance(CharacterLocation, Interactable->GetComponentLocation());
		float DistanceFromOrigin = DistanceFromCharacter;

		const bool bInHoverRangeCharacterRelative = DistanceFromCharacter <= Data->MaxSelectionDistanceFromCharacter;
		bool bInHoverRangeCameraRelative = true;

		if (DetectCameraProximity)
		{
			if (!(FVector::DotProduct(CharacterDirection, (Interactable->GetComponentLocation() - CharacterLocation).GetSafeNormal()) > 0))
			{
				continue;
			}

			FVector2d ScreenSize;
			GEngine->GameViewport->GetViewportSize(ScreenSize);
			const FVector2d ScreenCenter = ScreenSize/2;
			const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			
			FVector2d ScreenPosition;
			UGameplayStatics::ProjectWorldToScreen(PlayerController, Interactable->GetComponentLocation(), ScreenPosition);

			DistanceFromOrigin = FVector2D::Distance(ScreenCenter, ScreenPosition);
			bInHoverRangeCameraRelative = DistanceFromOrigin <= Data->MaxSelectionDistanceFromScreenCenter;
		}
		
		if (bInHoverRangeCharacterRelative && bInHoverRangeCameraRelative && DistanceFromOrigin <= TargetDistanceFromOrigin)
		{
			TargetDistanceFromOrigin = DistanceFromOrigin;
			NewSelectedComponent = Interactable;
		}
	}

	return NewSelectedComponent;
}

void UInputInteractorComponent::Interact(UInputInteractableComponent* Interactable)
{
	if (!IsValid(Interactable))
	{
		return;
	}

	if (!Interactable->IsActive())
	{
		return;
	}
	
	const UInputInteractableDataAsset* Data = Interactable->GetInteractableDataAsset();
	if (!IsValid(Data))
	{
		return;
	}

	if (Data->bIsHoldInteraction)
	{
		if (!SetCurrentInteractionTime(CurrentInteractionTime + CachedDeltatime, Interactable))
		{
			return;
		}
	}

	AActor* InteractorOwner = GetOwner();
	AActor* InteractableOwner = Interactable->GetOwner();

	if (!IsValid(InteractableOwner) || !IsValid(InteractableOwner))
	{
		return;
	}
	
	if (InteractableOwner->Implements<UInputInteractionOverride>() && IInputInteractionOverride::Execute_ShouldOverrideInteraction(InteractableOwner, InteractorOwner, Interactable))
	{
		return;
	}

	if (InteractorOwner->Implements<UInputInteractionOverride>() && IInputInteractionOverride::Execute_ShouldOverrideInteraction(InteractorOwner, InteractableOwner, Interactable))
	{
		return;
	}

	SetCurrentInteractionTime(0.f, Interactable);
	Interactable->OnInteract.Broadcast(InteractorOwner, InteractableOwner);
	OnInteract.Broadcast(Interactable);
}

bool UInputInteractorComponent::SetCurrentInteractionTime(const float NewInteractionTime,
	const UInputInteractableComponent* Interactable)
{
	if (!IsValid(Interactable))
	{
		return false;
	}

	const UInputInteractableDataAsset* Data = Interactable->GetInteractableDataAsset();
	if (!IsValid(Data))
	{
		return false;
	}
	
	if (TimeSinceInteraction <= Data->DelayBetweenInteraction)
	{
		return false;
	}
	
	CurrentInteractionTime = NewInteractionTime;
	const float Percent = NewInteractionTime / Data->InteractionDuration;

	if (Percent >= 1.f)
	{
		TimeSinceInteraction = 0.f;
		CurrentInteractionTime = 0.f;
		Interactable->OnInteractionPercentageUpdated.Broadcast(0.f);
		return true;
	}

	Interactable->OnInteractionPercentageUpdated.Broadcast(Percent);
	return false;
}

void UInputInteractorComponent::UpdateInteractableState(UInputInteractableComponent* Interactable,
	const EInteractionState NewState) const
{
	if (!IsValid(Interactable))
	{
		return;
	}

	AActor* InteractorOwner = GetOwner();
	AActor* InteractableOwner = Interactable->GetOwner();

	if (!IsValid(InteractableOwner) || !IsValid(InteractableOwner))
	{
		return;
	}
	
	if (InteractorOwner->Implements<UInputInteractionOverride>() && IInputInteractionOverride::Execute_ShouldOverrideState(InteractorOwner, InteractableOwner, Interactable))
	{
		return;
	}

	if (InteractableOwner->Implements<UInputInteractionOverride>() && IInputInteractionOverride::Execute_ShouldOverrideState(InteractableOwner, InteractorOwner, Interactable))
	{
		return;
	}

	Interactable->UpdateState(NewState);
}

bool UInputInteractorComponent::ExecuteObstructionTrace(const UInputInteractableComponent* Interactable) const
{
	if (!IsValid(Interactable))
	{
		UE_LOG(LogTemp, Warning, TEXT("Tried to execute obstruction trace but the interactable component is invalid"));
		return true;
	}

	const UInputInteractableDataAsset* Data = Interactable->GetInteractableDataAsset();
	if (!IsValid(Data))
	{
		return true;
	}

	if (Data->bDetectWhenObstructed && Data->bAllowInteractionWhenObstructed)
	{
		return false;
	}

	const APlayerController* Controller = Cast<APlayerController>(GetOwner()->GetInstigatorController());
	if (!IsValid(Controller))
	{
		UE_LOG(LogTemp, Warning, TEXT("Tried to execute obstruction trace but the player controller is invalid"));
		return true;
	}

	FVector StartLocation;
	FRotator StartRotation;
	Controller->GetActorEyesViewPoint(StartLocation, StartRotation);
	
	FHitResult ObstructionResult;
	const ETraceTypeQuery TraceTypeQuery =  UEngineTypes::ConvertToTraceType(Data->ObstructionTraceChannel);
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, Interactable->GetComponentLocation(), TraceTypeQuery, bTraceComplex, { GetOwner(), Interactable->GetOwner() }, DrawDebugType, ObstructionResult, bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
	
	return ObstructionResult.bBlockingHit;
}

void UInputInteractorComponent::UpdateSelectedInput()
{
	if (!SelectedInteractable)
	{
		return;
	}

	if (const UInputInteractableDataAsset* Data = SelectedInteractable->GetInteractableDataAsset();
		!IsValid(Data))
	{
		return;
	}
	
	for (const FGameplayTag& Key : ActiveKeys)
	{
		if (SelectedInteractable->GetInteractableDataAsset()->InputTag != Key)
		{
			continue;
		}
		
		Interact(SelectedInteractable);
	}
}

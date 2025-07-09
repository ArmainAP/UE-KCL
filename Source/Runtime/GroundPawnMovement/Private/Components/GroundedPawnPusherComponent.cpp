// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/GroundedPawnPusherComponent.h"

#include "AIController.h"
#include "KiraHelperLibrary.h"
#include "Misc/DataValidation.h"

#if WITH_EDITOR
EDataValidationResult UGroundedPawnPusherComponent::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (const UBlueprintGeneratedClass* BPGC = GetTypedOuter<UBlueprintGeneratedClass>();
		BPGC && !BPGC->IsChildOf(APawn::StaticClass()))
	{
		Context.AddError(NSLOCTEXT("Validation", "NotPawnClass", "Blueprint class owning this component must derive from APawn."));
		Result = EDataValidationResult::Invalid;
	}
	
	return Result;
}
#endif

UGroundedPawnPusherComponent::UGroundedPawnPusherComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bCanEverAffectNavigation = false;
}

void UGroundedPawnPusherComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = UKiraHelperLibrary::GetPawn(GetOwner());
}

void UGroundedPawnPusherComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!OwnerPawn.IsValid())
	{
		return;
	}

	if (!(OwnerPawn->GetLastMovementInputVector().Length() > 0))
	{
		return;
	}

	const float OwnerPawnMaxSpeed = UKiraHelperLibrary::GetMaxSpeed(OwnerPawn.Get());
	for (TWeakObjectPtr OverlappedGroundPushedComponent : OverlappedGroundPushedComponents)
	{
		if (!OverlappedGroundPushedComponent.IsValid())
		{
			continue;
		}

		const APawn* Pawn = UKiraHelperLibrary::GetPawn(OverlappedGroundPushedComponent->GetOwner());
		if (!Pawn)
		{
			continue;
		}
		
		FVector const OwnerActorLocation = OwnerPawn->GetActorLocation();
		FVector const OtherActorLocation = Pawn->GetActorLocation();

		FVector PushNormalVector = (OtherActorLocation - OwnerActorLocation).GetSafeNormal();
		PushNormalVector.Z = 0.0f;
		
		const FVector PushForceVector = PushNormalVector * PushForceMultiplier * OwnerPawnMaxSpeed * UKiraHelperLibrary::GetMass(Pawn);
		OverlappedGroundPushedComponent->Push(PushForceVector);
	}
}

UGroundedPawnPushedComponent* UGroundedPawnPusherComponent::GetGroundedPawnPushedComponent(AActor* Actor)
{
	const APawn* Pawn = UKiraHelperLibrary::GetPawn(Actor);
	return Pawn ? Pawn->GetComponentByClass<UGroundedPawnPushedComponent>() : nullptr;
}

void UGroundedPawnPusherComponent::HandleActorTraceBegin_Implementation(const FHitResult& HitResult)
{
	Super::HandleActorTraceBegin_Implementation(HitResult);

	OverlappedGroundPushedComponents.Add(GetGroundedPawnPushedComponent(HitResult.GetActor()));
}

void UGroundedPawnPusherComponent::HandleActorTraceEnd_Implementation(const FHitResult& HitResult)
{
	Super::HandleActorTraceEnd_Implementation(HitResult);

	OverlappedGroundPushedComponents.Remove(GetGroundedPawnPushedComponent(HitResult.GetActor()));
}
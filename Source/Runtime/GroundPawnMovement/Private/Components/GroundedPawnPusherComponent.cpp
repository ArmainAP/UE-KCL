// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/GroundedPawnPusherComponent.h"

#include "AIController.h"
#include "KiraHelperLibrary.h"
#include "KCL/Public/Misc/CollisionProfiles.h"
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
	SphereRadius = 100.0f;
	
	SetGenerateOverlapEvents(true);
	UPrimitiveComponent::SetCollisionProfileName(COLLISION_OVERLAP_ALL_DYNAMIC);
}

void UGroundedPawnPusherComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = UKiraHelperLibrary::GetPawn(GetOwner());
	OnComponentBeginOverlap.AddDynamic(this, &UGroundedPawnPusherComponent::OnBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UGroundedPawnPusherComponent::OnEndOverlap);
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
		
		const FVector PushForceVector = PushNormalVector * PushForceMultiplier * OwnerPawnMaxSpeed * CalculateMass();
		OverlappedGroundPushedComponent->Push(PushForceVector);
	}
}

UGroundedPawnPushedComponent* UGroundedPawnPusherComponent::GetGroundedPawnPushedComponent(AActor* Actor)
{
	const AAIController* AIC = UKiraHelperLibrary::GetAIController(Actor);
	return AIC ? AIC->GetComponentByClass<UGroundedPawnPushedComponent>() : nullptr;
}

void UGroundedPawnPusherComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OverlappedGroundPushedComponents.Add(GetGroundedPawnPushedComponent(OtherActor));
}

void UGroundedPawnPusherComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappedGroundPushedComponents.Remove(GetGroundedPawnPushedComponent(OtherActor));
}

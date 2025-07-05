// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/GroundedPawnPushComponent.h"

#include "AIController.h"
#include "KiraHelperLibrary.h"
#include "Components/GroundPathFollowingComponent.h"
#include "KCL/Public/Misc/CollisionProfiles.h"
#include "Misc/DataValidation.h"

#if WITH_EDITOR
EDataValidationResult UGroundedPawnPushComponent::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!Cast<APawn>(GetOwner()))
	{
		const FString Error = FString::Printf(TEXT("%s is not a child of %s"), *GetOwner()->GetName(), *APawn::StaticClass()->GetName());
		Context.AddError(FText::FromString(Error));
		Result = EDataValidationResult::Invalid;
	}
	
	return Result;
}
#endif

UGroundedPawnPushComponent::UGroundedPawnPushComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bCanEverAffectNavigation = false;
	SphereRadius = 100.0f;
	
	SetGenerateOverlapEvents(true);
	UPrimitiveComponent::SetCollisionProfileName(COLLISION_OVERLAP_ALL_DYNAMIC);
}

void UGroundedPawnPushComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = UKiraHelperLibrary::GetPawn(GetOwner());
	OnComponentBeginOverlap.AddDynamic(this, &UGroundedPawnPushComponent::OnBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UGroundedPawnPushComponent::OnEndOverlap);
}

void UGroundedPawnPushComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
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
	for (TWeakObjectPtr GroundPathFollowingComponent : OverlappedGroundPathFollowingComponents)
	{
		if (!GroundPathFollowingComponent.IsValid())
		{
			continue;
		}

		const APawn* Pawn = UKiraHelperLibrary::GetPawn(GroundPathFollowingComponent->GetOwner());
		if (!Pawn)
		{
			continue;
		}
		
		FVector const OwnerActorLocation = OwnerPawn->GetActorLocation();
		FVector const OtherActorLocation = Pawn->GetActorLocation();

		FVector PushNormalVector = (OtherActorLocation - OwnerActorLocation).GetSafeNormal();
		PushNormalVector.Z = 0.0f;
		
		const FVector PushForceVector = PushNormalVector * PushForceMultiplier * OwnerPawnMaxSpeed * CalculateMass();
		GroundPathFollowingComponent->Push(PushForceVector);
	}
}

UGroundPathFollowingComponent* UGroundedPawnPushComponent::GetGroundPathFollowingComponent(AActor* Actor)
{
	const AAIController* AIC = UKiraHelperLibrary::GetAIController(Actor);
	return AIC ? AIC->GetComponentByClass<UGroundPathFollowingComponent>() : nullptr;
}

void UGroundedPawnPushComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OverlappedGroundPathFollowingComponents.Add(GetGroundPathFollowingComponent(OtherActor));
}

void UGroundedPawnPushComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappedGroundPathFollowingComponents.Remove(GetGroundPathFollowingComponent(OtherActor));
}

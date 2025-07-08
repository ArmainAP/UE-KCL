// Copyright Kat Code Labs, SRL. All Rights Reserved.

#include "FunctionLibrary/GroundPawnMovementHelpers.h"
#include "KiraHelperLibrary.h"
#include "Components/GroundedPawnMovement.h"
#include "GameFramework/CharacterMovementComponent.h"

float UGroundPawnMovementHelpers::GetWalkableFloorZ(AActor* Actor)
{
	APawn* Pawn = UKiraHelperLibrary::GetPawn(Actor);
	if (!Pawn)
	{
		return 0.0f;
	}
	
	if (const UCharacterMovementComponent* CharacterMovementComponent = UKiraHelperLibrary::GetPawnMovementComponent<UCharacterMovementComponent>(Pawn))
	{
		return CharacterMovementComponent->GetWalkableFloorZ();
	}

	if (const UGroundedPawnMovement* GroundedPawnMovement = UKiraHelperLibrary::GetPawnMovementComponent<UGroundedPawnMovement>(Pawn))
	{
		return GroundedPawnMovement->GetWalkableFloorZ();
	}

	return 0.0f;
}

FVector UGroundPawnMovementHelpers::InterpVectorByRotation(const FVector& Current, const FVector& Target, const float InterpSpeed, const float DeltaTime)
{
	const FQuat CurrentRotation = FQuat::FindBetweenNormals(FVector::ForwardVector, Current.GetSafeNormal());
	const FQuat TargetRotation = FQuat::FindBetweenNormals(FVector::ForwardVector, Target.GetSafeNormal());
	const FQuat InterpRotation = FMath::QInterpTo(CurrentRotation, TargetRotation, DeltaTime, InterpSpeed);

	FVector InterpVector = InterpRotation.RotateVector(FVector::ForwardVector);
	InterpVector.Normalize();
	return InterpVector * Target.Length();
}

void UGroundPawnMovementHelpers::AddForce(APawn* Pawn, const FVector Force)
{
	if (UCharacterMovementComponent* CharacterMovementComponent = UKiraHelperLibrary::GetPawnMovementComponent<UCharacterMovementComponent>(Pawn))
	{
		CharacterMovementComponent->AddForce(Force);
	}

	if (UGroundedPawnMovement* GroundedPawnMovement = UKiraHelperLibrary::GetPawnMovementComponent<UGroundedPawnMovement>(Pawn))
	{
		GroundedPawnMovement->AddForce(Force);
	}
}

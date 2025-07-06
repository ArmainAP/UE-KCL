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

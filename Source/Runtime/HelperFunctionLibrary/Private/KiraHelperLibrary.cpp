// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "KiraHelperLibrary.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Components/ShapeComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

AAIController* UKiraHelperLibrary::GetAIController(AActor* Actor)
{
	if (const APawn* AsPawn = Cast<APawn>(Actor))
	{
		return Cast<AAIController>(AsPawn->GetController());
	}
	return Cast<AAIController>(Actor);
}

APawn* UKiraHelperLibrary::GetPawn(AActor* Actor)
{
	if (const AAIController* AsAIC = Cast<AAIController>(Actor))
	{
		return AsAIC->GetPawn();
	}
	return Cast<APawn>(Actor);
}

bool UKiraHelperLibrary::GetNavigablePathLenght(UWorld* WorldContextObject, const FVector& Start, const FVector& End, float& OutLength)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(WorldContextObject);
	if (!NavSys)
	{
		return false;
	}

	const ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
	if (!NavData)
	{
		return false;
	}

	FNavAgentProperties AgentProps;
	FPathFindingQuery Query;
	FPathFindingResult Result;

	Query = FPathFindingQuery(nullptr, *NavData, Start, End);
	Result = NavSys->FindPathSync(AgentProps, Query);
	if (!Result.IsSuccessful())
	{
		return false;
	}

	OutLength = Result.Path->GetLength();
	return true;
}

float UKiraHelperLibrary::GetMass(const AActor* Actor)
{
	UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
	return RootPrim ? RootPrim->CalculateMass() : 0.f;
}

float UKiraHelperLibrary::GetMaxSpeed(const APawn* Pawn)
{
	const UMovementComponent* OwnerPawnMovementComponent = Pawn->GetMovementComponent();
	return OwnerPawnMovementComponent ? OwnerPawnMovementComponent->GetMaxSpeed() : 0.0f;
}

bool UKiraHelperLibrary::GetFloorActor(AActor* TargetActor, FHitResult& OutHit, float TraceDistance)
{
	if (!TargetActor)
	{
		return false;
	}

	const UWorld* World = TargetActor->GetWorld();
	if (!World)
	{
		return false;
	}

	// Reuse the CharacterMovementComponent floor
	if (const UCharacterMovementComponent* Move = GetPawnMovementComponent<UCharacterMovementComponent>(GetPawn<ACharacter>(TargetActor));
		Move && Move->CurrentFloor.bBlockingHit)
	{
		OutHit = Move->CurrentFloor.HitResult;
		return true;
	}

	// Start just above the actor’s feet and sweep down.
	FVector Origin, Extent;
	TargetActor->GetActorBounds(true, Origin, Extent);
	const float HalfHeight = Extent.Z;
	const FVector Start = Origin - FVector(0, 0, HalfHeight * 0.98f);
	const FVector End = Start - FVector(0, 0, HalfHeight + TraceDistance);
	const FCollisionQueryParams Params(TEXT("GetFloor"), false, TargetActor);
	
	// Prefer capsule sweep if the actor owns one.
	if (const UShapeComponent* Capsule = TargetActor->FindComponentByClass<UShapeComponent>())
	{
		return World->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, ECC_Visibility, Capsule->GetCollisionShape(), Params);
	}

	return World->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params);
}



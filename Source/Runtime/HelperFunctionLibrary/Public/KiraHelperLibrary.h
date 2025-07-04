// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KiraHelperLibrary.generated.h"

class AAIController;

UCLASS()
class HELPERFUNCTIONLIBRARY_API UKiraHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "KiraHelperLibrary")
	static AAIController* GetAIController(AActor* Actor);

	template<class T>
	static T* GetAIController(AActor* Actor)
	{
		return Cast<T>(GetAIController(Actor));
	}
	
	UFUNCTION(BlueprintPure, Category = "KiraHelperLibrary")
	static APawn* GetPawn(AActor* Actor);

	template<class T>
	static T* GetPawn(AActor* Actor)
	{
		return Cast<T>(GetPawn(Actor));
	}

	UFUNCTION(BlueprintPure, Category = "KiraHelperLibrary")
	static bool GetNavigablePathLenght(UWorld* WorldContextObject, const FVector& Start, const FVector& End, float& OutLength);

	UFUNCTION(BlueprintPure, Category = "KiraHelperLibrary")
	static float GetMass(const AActor* Actor);

	UFUNCTION(BlueprintPure, Category = "KiraHelperLibrary")
	static float GetMaxSpeed(const APawn* Pawn);

	template<class T>
	static T* GetPawnMovementComponent(APawn* Pawn)
	{
		return Pawn ? Cast<T>(Pawn->GetMovementComponent()) : nullptr;
	}

	/** Return the actor the supplied pawn/actor is currently standing on.
	*  @param TargetActor     The actor you want to query.
	*  @param OutHit          Optional – full hit info for the floor.
	*  @param TraceDistance   Extra distance below the feet to look (cm).
	*  @return                The floor actor, or nullptr if nothing solid underfoot. */
	UFUNCTION(BlueprintPure, Category="Ground")
	static bool GetFloorActor(AActor* TargetActor, FHitResult& OutHit, float TraceDistance = 5.0f);
};

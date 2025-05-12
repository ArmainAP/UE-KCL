// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SightedComponent.h"
#include "Components/SighterComponent.h"
#include "Data/SightQueryContext.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SightSystemFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SIGHTSYSTEM_API USightSystemFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Sight System")
	static FGenericTeamId GetComponentTeamId(const UActorComponent* Component);

	UFUNCTION(BlueprintPure, Category = "Sight System")
	static bool ShouldSight(const USighterComponent* SighterComponent, const USightedComponent* SightedComponent);

	UFUNCTION(BlueprintPure, Category = "Sight System")
	static bool IsVisibleInsideCone(FSightQueryContext& Query, const float MaxRadius, const float FieldOfView);

	UFUNCTION(BlueprintCallable, Category = "Sight System")
	static void EvaluateQueryVisibility(FSightQueryContext& Query);
};

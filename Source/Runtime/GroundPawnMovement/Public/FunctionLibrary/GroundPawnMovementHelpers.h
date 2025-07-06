// Copyright Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveFloat.h"
#include "GroundPawnMovementHelpers.generated.h"

UCLASS()
class GROUNDPAWNMOVEMENT_API UGroundPawnMovementHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static float GetWalkableFloorZ(AActor* Actor);

	UFUNCTION(BlueprintPure)
	static FVector InterpVectorByRotation(const FVector& Current, const FVector& Target, float InterpSpeed, float DeltaTime);
};
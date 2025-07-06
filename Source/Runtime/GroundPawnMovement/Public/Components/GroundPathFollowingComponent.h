// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"
#include "Structs/GroundPathFollowingSineWave.h"
#include "Structs/GroundPathFollowingSpeedVariation.h"
#include "GroundPathFollowingComponent.generated.h"

class UGroundedPathFollowingData;
/**
 * 
 */
UCLASS()
class GROUNDPAWNMOVEMENT_API UGroundPathFollowingComponent : public UPathFollowingComponent
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Push(FVector PushVector);
	virtual void Push_Implementation(FVector PushVector) { };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGroundPathFollowingSpeedVariation SpeedVariation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGroundPathFollowingSineWave SineWave;
};

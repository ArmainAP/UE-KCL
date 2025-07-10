// Copyright Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structs/GroundPawnSensor.h"
#include "GroundedPawnAvoidanceSensingData.generated.h"

/**
 * 
 */
UCLASS()
class GROUNDPAWNMOVEMENT_API UGroundedPawnAvoidanceSensingData : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Tunable parameters driving sensing behaviour. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGroundPawnSensorSettings Settings;
};

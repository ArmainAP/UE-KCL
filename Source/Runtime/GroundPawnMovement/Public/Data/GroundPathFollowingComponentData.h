// Copyright Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structs/GroundPathFollowingSettings.h"
#include "Structs/GroundPathFollowingSineWave.h"
#include "Structs/GroundPathFollowingSpeedVariation.h"
#include "Structs/GroundPathTrajectory.h"
#include "GroundPathFollowingComponentData.generated.h"

/**
 * 
 */
UCLASS()
class GROUNDPAWNMOVEMENT_API UGroundPathFollowingComponentData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGroundPathFollowingSettings PathFollowingSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGroundPathTrajectory Trajectory;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGroundPathFollowingSpeedVariation SpeedVariation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGroundPathFollowingSineWave SineWave;
};

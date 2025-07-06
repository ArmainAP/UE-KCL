// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GroundPathFollowingSpeedVariation.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct GROUNDPAWNMOVEMENT_API FGroundPathFollowingSpeedVariation
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnabled = true;

	// Minimum time between each variation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinTimeVariation = 1;

	// Maximum time between each variation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxTimeVariation = 3;

	// Minimum speed percentage that can be reduced randomly. The speed just slow down compared to max speed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(UIMin="0", UIMax="1"))
	float MinPercentVariation = 0.0f;
	
	// Maximum speed percentage that can be reduced randomly. The speed just slow down compared to max speed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(UIMin="0", UIMax="1"))
	float MaxPercentVariation = 0.3f;

	// How fast does the speed variation happens. Higher values mean faster change. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0))
	float InterpolationSpeed = 12.0f;

	void Update(const float DeltaTime)
	{
		if (!bEnabled)
		{
			return;
		}

		if (CurrentTimer >= NextTimer)
		{
			NextTimer = FMath::RandRange(MinTimeVariation, MaxTimeVariation);
			TargetSpeed = 1 - FMath::RandRange(MinPercentVariation, MaxPercentVariation);
			CurrentTimer = 0;
		}

		CurrentSpeed = FMath::FInterpConstantTo(CurrentSpeed, TargetSpeed, DeltaTime, InterpolationSpeed);
		CurrentTimer += DeltaTime;
	}

	float Get() const
	{
		return bEnabled ? CurrentSpeed : 1.0f;
	}

	void ResetSpeed()
	{
		TargetSpeed = 0.0f;
		CurrentSpeed = 0.0f;
	}

	void ResetTimer()
	{
		NextTimer = 0;
		CurrentTimer = 0;
	}

protected:
	float CurrentTimer = 0.0f;
	float NextTimer = 0.0f;
	float TargetSpeed = 0.0f;
	float CurrentSpeed = 0.0f;
};

// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SightVisionSettings.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct SIGHTSYSTEM_API FSightVisionSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	float Radius = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	float FieldOfView = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	float GainRateMultiplier = 1.0f;
};

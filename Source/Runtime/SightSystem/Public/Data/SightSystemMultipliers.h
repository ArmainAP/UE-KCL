// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SightSystemMultipliers.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct SIGHTSYSTEM_API FSightSystemMultipliers
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	float GainRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	float GainDecayRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	float ForgetTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	float WaitTime = 1.0f;
};

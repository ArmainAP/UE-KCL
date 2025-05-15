// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AIPerceptionTypes.h"
#include "SightSystemDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class SIGHTSYSTEM_API USightSystemDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	/* Filter who can be detected (enemy, neutral, friend) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	FAISenseAffiliationFilter DetectionByAffiliation = FAISenseAffiliationFilter(true, true, true);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	float Radius = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	float FieldOfView = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	float GainRateMultiplier = 1.0f;

	/* Collision channel used for line-of-sight traces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	bool bTraceComplex = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	TEnumAsByte<ECollisionChannel> SightCollisionChannel = ECC_WorldStatic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugTrace = EDrawDebugTrace::None;

	/* Spotted → Perceived time (sec). Final time = GainSightDuration / GainMultiplier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	float PerceptionGainTime = 1.0f;

	/* Decay rate of accumulated gain (1 = no decay) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	float GainDecayRate = 1.0f;

	/* Grace period (sec) before a Spotted target becomes Lost */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	float SpottedGraceTime = 1.0f;
	
	/* Distance (cm) target must move from last-known location to auto-fail sight */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	float LostSightToleranceDist = 0.0f;

	/* Max range (cm) for losing sight checks */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	float LoseSightDistance = 1250.0f;

	/* FOV (deg) used when checking for lost sight */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	float LoseSightFieldOfView = 360.0f;

	/* Cooldown (sec) after losing sight before another check */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	float LoseSightCooldownTime = 1.0f;
};
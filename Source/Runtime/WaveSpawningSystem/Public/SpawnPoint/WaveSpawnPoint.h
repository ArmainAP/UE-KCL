// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "WaveSpawnPoint.generated.h"

/**
 * 
 */
UCLASS()
class WAVESPAWNINGSYSTEM_API AWaveSpawnPoint : public ATargetPoint
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual FTransform GetSpawnPointTransform();
};

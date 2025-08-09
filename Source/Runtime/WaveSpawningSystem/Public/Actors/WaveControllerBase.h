// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveControllerBase.generated.h"

UCLASS(Abstract)
class WAVESPAWNINGSYSTEM_API AWaveControllerBase : public AActor
{
	GENERATED_BODY()

public:
	AWaveControllerBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, BlueprintNativeEvent)
	AActor* GetSpawnPoint();
};

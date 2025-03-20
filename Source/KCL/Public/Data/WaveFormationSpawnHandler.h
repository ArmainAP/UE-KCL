// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/WaveSpawnHandlerDataAsset.h"
#include "WaveFormationSpawnHandler.generated.h"

class UFormationDataAsset;

/**
 * 
 */
UCLASS()
class KCL_API UWaveFormationSpawnHandler : public UWaveSpawnHandlerDataAsset
{
	GENERATED_BODY()

	virtual void OnSpawnActor_Implementation() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UFormationDataAsset* FormationDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 1, UIMin = 1))
	int FormationCount = 1;
};

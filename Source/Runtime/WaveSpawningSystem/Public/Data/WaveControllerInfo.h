// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BatchSpawnData.h"
#include "WaveControllerInfo.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FWaveControllerInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentWave = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FBatchSpawnData> BatchSpawnData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SpawnedActorsDestroyedCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ActiveBatchSpawnerCount = 0;
};
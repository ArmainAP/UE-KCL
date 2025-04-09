// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BatchSpawnData.h"
#include "WaveSpawnData.h"
#include "WaveControllerInfo.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FWaveControllerInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FWaveSpawnData WaveSpawnData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentWave = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SpawnedActorsDestroyedCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<UWaveSpawnHandlerDataAsset*> ActiveBatchSpawners;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaveCountdown = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentWaveActorCount = 0;
};
// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BatchSpawnData.generated.h"

class UWaveSpawnHandler;
class UWaveSpawnHandlerDataAsset;

/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct FBatchSpawnData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BatchDelay = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SpawnPointID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> SpawnedActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SpawnCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWaveSpawnHandler> SpawnHandler;
};

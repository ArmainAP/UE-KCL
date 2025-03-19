// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BatchSpawnData.h"
#include "StructUtils/InstancedStruct.h"
#include "WaveSpawnData.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable, BlueprintType)
struct FWaveSpawnData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TInstancedStruct<FBatchSpawnData>> BatchSpawnData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float WaveDelay = 3;
};

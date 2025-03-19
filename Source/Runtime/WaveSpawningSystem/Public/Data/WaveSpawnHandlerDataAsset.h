// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BatchSpawnData.h"
#include "Engine/DataAsset.h"
#include "SpawnPoint/WaveSpawnPoint.h"
#include "WaveSpawnHandlerDataAsset.generated.h"

// Delegate for when an actor is spawned
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorSpawnedEvent, AActor*, SpawnedActor);

// Delegate for when the batch spawn completes
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBatchSpawnCompleteEvent);

/**
 * 
 */
UCLASS()
class WAVESPAWNINGSYSTEM_API UWaveSpawnHandlerDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	void BeginSpawn(AWaveSpawnPoint* InSpawnPoint, const FBatchSpawnData& InBatchSpawnData);
	void CancelSpawn();

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnSpawnActor();

public:
	/** A delegate called when an actor is spawned. */
	UPROPERTY(BlueprintAssignable)
	FActorSpawnedEvent OnActorSpawned;
	
	/** A delegate called when the batch spawn completes. */
	UPROPERTY(BlueprintAssignable)
	FBatchSpawnCompleteEvent OnBatchComplete;

protected:
	UPROPERTY()
	UWorld* World;
	
	UPROPERTY()
	AWaveSpawnPoint* SpawnPoint;
	
	UPROPERTY()
	FBatchSpawnData BatchSpawnData;

	UPROPERTY()
	int SpawnedCount = 0;

	UPROPERTY()
	FTimerHandle SpawnTimer;
};

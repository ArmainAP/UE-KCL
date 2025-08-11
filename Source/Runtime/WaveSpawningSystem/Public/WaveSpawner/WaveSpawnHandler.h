// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/BatchSpawnData.h"
#include "SpawnHandlers/SpawnHandler.h"
#include "WaveSpawnHandler.generated.h"

// Delegate for when the batch spawn completes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBatchSpawnCompleteEvent, UWaveSpawnHandler*, BatchSpawner);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class WAVESPAWNINGSYSTEM_API UWaveSpawnHandler : public USpawnHandler
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetSpawnData(const FBatchSpawnData& InBatchSpawnData);

protected:
	virtual void RequestSpawn_Implementation() override;
	virtual void BeginSpawnActor_Implementation() override;
	virtual void PostSpawnActor_Implementation(AActor* Actor) override;

public:
	/** A delegate called when the batch spawn completes. */
	UPROPERTY(BlueprintAssignable)
	FBatchSpawnCompleteEvent OnBatchComplete;

protected:
	UPROPERTY()
	FBatchSpawnData BatchSpawnData;

	UPROPERTY()
	int SpawnedCount = 0;
};

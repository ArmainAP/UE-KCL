// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/BatchSpawnData.h"
#include "Engine/CancellableAsyncAction.h"
#include "SpawnPoint/WaveSpawnPoint.h"
#include "WaveSpawnHandler.generated.h"

// Delegate for when the batch spawn completes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBatchSpawnCompleteEvent, UWaveSpawnHandler*, BatchSpawner);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class WAVESPAWNINGSYSTEM_API UWaveSpawnHandler : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;
	virtual void Activate() override;
	virtual void Cancel() override;

	UFUNCTION(BlueprintCallable)
	void SetSpawnData(const AWaveSpawnPoint* InSpawnPoint, const FBatchSpawnData& InBatchSpawnData);

	UFUNCTION(BlueprintNativeEvent)
	void BeginSpawn();

	UFUNCTION(BlueprintNativeEvent)
	void SpawnActor(const FTransform& Transform);

	UFUNCTION(BlueprintNativeEvent)
	void PostSpawnActor(AActor* Actor);

	/** A delegate called when an actor is spawned. */
	UPROPERTY(BlueprintAssignable)
	FActorSpawnedEvent OnActorSpawned;

	/** A delegate called when the batch spawn completes. */
	UPROPERTY(BlueprintAssignable)
	FBatchSpawnCompleteEvent OnBatchComplete;

protected:
	void ClearTimers();

	UFUNCTION()
	void OnSpawnTimeout();

	UPROPERTY()
	TObjectPtr<const AWaveSpawnPoint> SpawnPoint;

	UPROPERTY()
	FBatchSpawnData BatchSpawnData;

	UPROPERTY()
	int SpawnedCount = 0;

	UPROPERTY()
	FTimerHandle SpawnTimer;
};

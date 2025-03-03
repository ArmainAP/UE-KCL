#pragma once

#include "CoreMinimal.h"
#include "Data/BatchSpawnData.h"
#include "Engine/CancellableAsyncAction.h"
#include "BatchSpawnLatentAction.generated.h"

class AWaveSpawnPoint;
// Delegate for when an actor is spawned
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorSpawnedEvent, AActor*, SpawnedActor);

// Delegate for when the batch spawn completes
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBatchSpawnCompleteEvent);

/**
 * Latent action class for batch spawning with delay and interval
 */
UCLASS()
class WAVESPAWNINGSYSTEM_API UBatchSpawnLatentAction : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, DisplayName="Batch Spawn", meta=(WorldContext="WorldContext", BlueprintInternalUseOnly="true"))
	static UBatchSpawnLatentAction* BatchSpawnAsyncAction(const UObject* WorldContext, const FBatchSpawnData BatchSpawnData, AWaveSpawnPoint* SpawnPoint);

	// Start UCancellableAsyncAction Functions
	virtual void Activate() override;
	virtual void Cancel() override;
	// End UCancellableAsyncAction Functions
	
	/** A delegate called when an actor is spawned. */
	UPROPERTY(BlueprintAssignable)
	FActorSpawnedEvent OnActorSpawned;
	
	/** A delegate called when the batch spawn completes. */
	UPROPERTY(BlueprintAssignable)
	FBatchSpawnCompleteEvent OnBatchComplete;

private:
	void SpawnActor();
	
	UPROPERTY()
	TWeakObjectPtr<UWorld> ContextWorld = nullptr;
	
	UPROPERTY()
	FBatchSpawnData BatchSpawnData;
	
	UPROPERTY()
	TWeakObjectPtr<AWaveSpawnPoint> SpawnPoint;

	UPROPERTY()
	int SpawnedCount = 0;

	UPROPERTY()
	FTimerHandle SpawnTimer;
};

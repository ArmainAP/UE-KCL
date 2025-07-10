// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WaveSpawnPoint.generated.h"

// Delegate for when an actor is spawned
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorSpawnedEvent, AActor*, SpawnedActor);

/**
 * 
 */
UCLASS()
class WAVESPAWNINGSYSTEM_API AWaveSpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	AWaveSpawnPoint();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual FTransform GetSpawnPointTransform() const;

	/** A delegate called when an actor is spawned. */
	UPROPERTY(BlueprintAssignable)
	FActorSpawnedEvent OnActorSpawned;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> SceneRootComponent;
};

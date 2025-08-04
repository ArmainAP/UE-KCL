// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SpawnHandlers/SpawnHandler.h"
#include "WaveSpawnPoint.generated.h"

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

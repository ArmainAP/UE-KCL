// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/WorldActorSpawner.h"
#include "Interfaces/WorldActorSpawnerInterface.h"
#include "WorldActorSpawnerVolume.generated.h"

class UBoxComponent;
/**
 * 
 */
UCLASS()
class SPAWNHELPERS_API AWorldActorSpawnerVolume : public AWorldActorSpawner, public IWorldActorSpawnerInterface
{
	GENERATED_BODY()

public:
	AWorldActorSpawnerVolume(const FObjectInitializer& ObjectInitializer);

	virtual FTransform GetSpawnActorTransform_Implementation() const override;

protected:
	UPROPERTY(EditAnywhere, Category=Components, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* SpawnVolumeBox;
};

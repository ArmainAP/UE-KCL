// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SpawnPoint/WaveSpawnPoint.h"
#include "WaveSpawnPointVolume.generated.h"

class UBoxComponent;
/**
 * 
 */
UCLASS()
class WAVESPAWNINGSYSTEM_API AWaveSpawnPointVolume : public AWaveSpawnPoint
{
	GENERATED_BODY()

	AWaveSpawnPointVolume();
	
public:
	virtual FTransform GetSpawnPointTransform() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* SpawnVolumeBox;
};

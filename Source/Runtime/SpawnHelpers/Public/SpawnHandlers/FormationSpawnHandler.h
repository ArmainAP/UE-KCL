// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WaveSpawner/WaveSpawnHandler.h"
#include "FormationSpawnHandler.generated.h"

class UFormationDataAsset;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class SPAWNHELPERS_API UFormationSpawnHandler : public USpawnHandler
{
	GENERATED_BODY()

protected:
	virtual void BeginSpawnActor_Implementation() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UFormationDataAsset* FormationDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 1, UIMin = 1))
	int FormationCount = 1;
};

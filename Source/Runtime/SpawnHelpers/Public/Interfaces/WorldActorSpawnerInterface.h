// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WorldActorSpawnerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWorldActorSpawnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPAWNHELPERS_API IWorldActorSpawnerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FTransform GetSpawnActorTransform() const;
};
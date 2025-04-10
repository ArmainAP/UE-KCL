// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "WaveSpawner/WaveSpawnHandler.h"
#include "WaveSpawningSystemSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Engine, DefaultConfig)
class WAVESPAWNINGSYSTEM_API UWaveSpawningSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UWaveSpawningSystemSettings();
	TSubclassOf<UWaveSpawnHandler> GetDefaultSpawnHandlerClass() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config)
	TSubclassOf<UWaveSpawnHandler> DefaultSpawnHandler = UWaveSpawnHandler::StaticClass();
};

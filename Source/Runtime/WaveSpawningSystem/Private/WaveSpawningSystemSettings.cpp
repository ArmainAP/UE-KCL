// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "WaveSpawningSystemSettings.h"

UWaveSpawningSystemSettings::UWaveSpawningSystemSettings()
{
	CategoryName = "KCL";
	SectionName = "Wave Spawning System";
}

TSubclassOf<UWaveSpawnHandler> UWaveSpawningSystemSettings::GetDefaultSpawnHandlerClass() const
{
	return DefaultSpawnHandler;
}

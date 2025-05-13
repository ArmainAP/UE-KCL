// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "SpawnPoint/WaveSpawnPoint.h"

AWaveSpawnPoint::AWaveSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

FTransform AWaveSpawnPoint::GetSpawnPointTransform() const
{
	return GetActorTransform();
}

// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "SpawnPoint/WaveSpawnPoint.h"

FTransform AWaveSpawnPoint::GetSpawnPointTransform()
{
	return GetActorTransform();
}

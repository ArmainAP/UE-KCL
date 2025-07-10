// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "SpawnPoint/WaveSpawnPoint.h"

AWaveSpawnPoint::AWaveSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(GET_MEMBER_NAME_CHECKED(AWaveSpawnPoint, SceneRootComponent));
	SetRootComponent(SceneRootComponent);
}

FTransform AWaveSpawnPoint::GetSpawnPointTransform() const
{
	return GetActorTransform();
}

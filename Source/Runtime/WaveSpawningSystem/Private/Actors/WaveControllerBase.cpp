// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Actors/WaveControllerBase.h"

AWaveControllerBase::AWaveControllerBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

AActor* AWaveControllerBase::GetSpawnPoint_Implementation()
{
	return this;
}

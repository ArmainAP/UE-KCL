// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "WaveSpawner/WaveSpawnHandler.h"

#include "Data/BatchSpawnData.h"

void UWaveSpawnHandler::SetSpawnData(const FBatchSpawnData& InBatchSpawnData)
{
	BatchSpawnData = InBatchSpawnData;
	SpawnInterval = BatchSpawnData.SpawnInterval;
	FirstSpawnDelay = BatchSpawnData.BatchDelay;
	ActorToSpawn = BatchSpawnData.SpawnedActor;
	SpawnCount = BatchSpawnData.SpawnCount;
}

void UWaveSpawnHandler::RequestSpawn_Implementation()
{
	if (SpawnedCount >= BatchSpawnData.SpawnCount)
	{
		OnBatchComplete.Broadcast(this);
		Cancel();
		return;
	}

	Super::RequestSpawn_Implementation();
}

void UWaveSpawnHandler::BeginSpawnActor_Implementation()
{
	SpawnActor(GetSpawnActorTransform());
}

void UWaveSpawnHandler::PostSpawnActor_Implementation(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}
	
	SpawnedCount++;
	OnActorSpawned.Broadcast(Actor);
}

// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "WaveSpawner/WaveSpawnHandler.h"

#include "Data/BatchSpawnData.h"
#include "SpawnPoint/WaveSpawnPoint.h"

void UWaveSpawnHandler::SetSpawnData(const AWaveSpawnPoint* InSpawnPoint, const FBatchSpawnData& InBatchSpawnData)
{
	SpawnPoint = InSpawnPoint;
	BatchSpawnData = InBatchSpawnData;
	SpawnInterval = BatchSpawnData.SpawnInterval;
	FirstSpawnDelay = BatchSpawnData.BatchDelay;
	ActorToSpawn = BatchSpawnData.SpawnedActor;
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

FTransform UWaveSpawnHandler::GetSpawnActorTransform_Implementation() const
{
	check(SpawnPoint)
	return SpawnPoint->GetSpawnPointTransform();
}

void UWaveSpawnHandler::PostSpawnActor_Implementation(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}
	
	SpawnedCount++;
	OnActorSpawned.Broadcast(Actor);
	if (SpawnPoint)
	{
		SpawnPoint->OnActorSpawned.Broadcast(Actor);
	}
}
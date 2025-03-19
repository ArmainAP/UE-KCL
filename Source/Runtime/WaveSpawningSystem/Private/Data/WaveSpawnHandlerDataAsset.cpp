// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Data/WaveSpawnHandlerDataAsset.h"
#include "SpawnPoint/WaveSpawnPoint.h"

void UWaveSpawnHandlerDataAsset::BeginSpawn(AWaveSpawnPoint* InSpawnPoint, const FBatchSpawnData& InBatchSpawnData)
{
	if (!InSpawnPoint)
	{
		return;
	}
	
	CancelSpawn();
	
	SpawnPoint = InSpawnPoint;
	World = SpawnPoint->GetWorld();
	BatchSpawnData = InBatchSpawnData;

	if (World)
	{
		// Set up the spawn timer
		World->GetTimerManager().SetTimer(
			SpawnTimer,
			FTimerDelegate::CreateUObject(this, &UWaveSpawnHandlerDataAsset::OnSpawnActor),
			BatchSpawnData.SpawnInterval,
			true, BatchSpawnData.BatchDelay);
	}
	else
	{
		CancelSpawn();
	}
}

void UWaveSpawnHandlerDataAsset::CancelSpawn()
{
	if (World)
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.ClearTimer(SpawnTimer);
	}
}

void UWaveSpawnHandlerDataAsset::OnSpawnActor_Implementation()
{
	if (!IsValid(World))
	{
		CancelSpawn();
		return;
	}

	// Spawn the actor
	if (BatchSpawnData.SpawnedActor && SpawnedCount < BatchSpawnData.SpawnCount)
	{
		AActor* SpawnedActor = World->SpawnActor<AActor>(BatchSpawnData.SpawnedActor, SpawnPoint->GetSpawnPointTransform());
		OnActorSpawned.Broadcast(SpawnedActor);
		SpawnedCount++;
	}
	else // Complete the action
	{
		CancelSpawn();
		OnBatchComplete.Broadcast();
	}
}

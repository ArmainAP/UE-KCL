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
	SpawnedCount = 0;
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

AActor* UWaveSpawnHandlerDataAsset::SpawnActor(const FTransform& Transform)
{
	if (!IsValid(World))
	{
		return nullptr;
	}
	
	AActor* SpawnedActor = World->SpawnActor<AActor>(BatchSpawnData.SpawnedActor, Transform);
	OnActorSpawned.Broadcast(SpawnedActor);
	SpawnedCount++;
	return SpawnedActor;
}

void UWaveSpawnHandlerDataAsset::OnSpawnActor_Implementation()
{
	if (!IsValid(SpawnPoint) || !BatchSpawnData.SpawnedActor)
	{
		CancelSpawn();
		return;
	}

	if (SpawnedCount >= BatchSpawnData.SpawnCount)
	{
		CancelSpawn();
		OnBatchComplete.Broadcast();
		return;
	}
	
	SpawnActor(SpawnPoint->GetSpawnPointTransform());
}

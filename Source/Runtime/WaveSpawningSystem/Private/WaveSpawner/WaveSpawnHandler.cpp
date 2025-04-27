// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "WaveSpawner/WaveSpawnHandler.h"

#include "Data/BatchSpawnData.h"
#include "SpawnPoint/WaveSpawnPoint.h"

UWorld* UWaveSpawnHandler::GetWorld() const
{
	return IsValid(SpawnPoint) ? SpawnPoint->GetWorld() : nullptr;
}

void UWaveSpawnHandler::Activate()
{
	const UWorld* World = GetWorld();
	if (IsValid(World))
	{
		// Set up the spawn timer
		World->GetTimerManager().SetTimer(
			SpawnTimer,
			FTimerDelegate::CreateUObject(this, &UWaveSpawnHandler::OnSpawnTimeout),
			BatchSpawnData.SpawnInterval,
			true, BatchSpawnData.BatchDelay);
	}
	else
	{
		Cancel();
	}
}

void UWaveSpawnHandler::Cancel()
{
	ClearTimers();
	Super::Cancel();
}

void UWaveSpawnHandler::SetSpawnData(const AWaveSpawnPoint* InSpawnPoint, const FBatchSpawnData& InBatchSpawnData)
{
	SpawnPoint = InSpawnPoint;
	BatchSpawnData = InBatchSpawnData;
}

void UWaveSpawnHandler::BeginSpawn_Implementation()
{
	if (!IsValid(SpawnPoint))
	{
		Cancel();
		return;
	}
	
	SpawnActor(SpawnPoint->GetSpawnPointTransform());
}

void UWaveSpawnHandler::SpawnActor_Implementation(const FTransform& Transform)
{
	if (UWorld* World = GetWorld(); IsValid(World) && BatchSpawnData.SpawnedActor)
	{
		PostSpawnActor(World->SpawnActor<AActor>(BatchSpawnData.SpawnedActor, Transform));
	}
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

void UWaveSpawnHandler::ClearTimers()
{
	const UWorld* World = GetWorld();
	if (IsValid(World))
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.ClearTimer(SpawnTimer);	
	}
}

void UWaveSpawnHandler::OnSpawnTimeout()
{
	if (SpawnedCount >= BatchSpawnData.SpawnCount)
	{
		OnBatchComplete.Broadcast(this);
		Cancel();
		return;
	}

	BeginSpawn();
}

#include "BatchSpawnLatentAction.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "SpawnPoint/WaveSpawnPoint.h"

UBatchSpawnLatentAction* UBatchSpawnLatentAction::BatchSpawnAsyncAction(const UObject* WorldContext, const FBatchSpawnData BatchSpawnData, AWaveSpawnPoint* SpawnPoint)
{
	UWorld* ContextWorld = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
	if (!ContextWorld)
	{
		return nullptr;
	}

	UBatchSpawnLatentAction* NewAction = NewObject<UBatchSpawnLatentAction>();
	NewAction->ContextWorld = ContextWorld;
	NewAction->BatchSpawnData = BatchSpawnData;
	NewAction->SpawnPoint = SpawnPoint;
	NewAction->RegisterWithGameInstance(ContextWorld->GetGameInstance());

	return NewAction;
}

void UBatchSpawnLatentAction::Activate()
{
	if (ContextWorld.IsValid())
	{
		// Set up the spawn timer
		ContextWorld->GetTimerManager().SetTimer(
			SpawnTimer,
			FTimerDelegate::CreateUObject(this, &UBatchSpawnLatentAction::SpawnActor),
			BatchSpawnData.SpawnInterval,
			true, BatchSpawnData.BatchDelay);
	}
	else
	{
		Cancel();
	}
}

void UBatchSpawnLatentAction::Cancel()
{
	Super::Cancel();

	if (ContextWorld.IsValid())
	{
		FTimerManager& TimerManager = ContextWorld->GetTimerManager();
		TimerManager.ClearTimer(SpawnTimer);
	}
}

void UBatchSpawnLatentAction::SpawnActor()
{
	if (!IsActive() || !ContextWorld.IsValid())
	{
		Cancel();
		return;
	}

	// Spawn the actor
	if (SpawnedCount < BatchSpawnData.SpawnCount && BatchSpawnData.SpawnedActor)
	{
		AActor* SpawnedActor = ContextWorld->SpawnActor<AActor>(BatchSpawnData.SpawnedActor, SpawnPoint->GetSpawnPointTransform());
		OnActorSpawned.Broadcast(SpawnedActor);
		SpawnedCount++;
	}
	else // Complete the action
	{
		if (ContextWorld.IsValid())
		{
			ContextWorld->GetTimerManager().ClearTimer(SpawnTimer);
		}
		OnBatchComplete.Broadcast();
		Cancel();
	}
}

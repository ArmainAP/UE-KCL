// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "WaveSpawner/WaveSpawnerController.h"

#include "BatchSpawnLatentAction.h"
#include "Logging.h"
#include "Data/BatchSpawnData.h"
#include "Data/WaveSpawnData.h"

// Sets default values
AWaveSpawnerController::AWaveSpawnerController() : WaveDataTable(nullptr)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AWaveSpawnerController::BeginPlay()
{
	Super::BeginPlay();

	if (WaveDataTable)
	{
		ActivateWaves(WaveDataTable);
	}
}

void AWaveSpawnerController::ActivateWaves(UDataTable* DataTable)
{
	WaveDataTable = DataTable;
	if (WaveDataTable)
	{
		CachedTableRows = WaveDataTable->GetRowNames();
		OnBeginWaves.Broadcast(this);
		BeginWave();
	}
	else
	{
		UE_LOG(WaveSpawningSystemLog, Error, TEXT("WaveSpawnerController::ActivateWaves - WaveDataTable is NULL"));
	}
}

void AWaveSpawnerController::BeginWave_Implementation()
{
	WaveInfo.CurrentWave++;
	
	if (!CachedTableRows.IsValidIndex(WaveInfo.CurrentWave))
	{
		UE_LOG(WaveSpawningSystemLog, Error, TEXT("WaveSpawnerController::ActivateWaves - %d is not valid index for CachedTableRows"), WaveInfo.CurrentWave);
		return;
	}

	const FName RowName = CachedTableRows[WaveInfo.CurrentWave];
	const FWaveSpawnData* WaveSpawnData = WaveDataTable->FindRow<FWaveSpawnData>(RowName, TEXT("AWaveSpawnerController::BeginWave"));
	if (!WaveSpawnData)
	{
		UE_LOG(WaveSpawningSystemLog, Error, TEXT("WaveSpawnerController::ActivateWaves - Unable to find data table row %s"), *RowName.ToString());
		return;
	}
		
	WaveInfo.ActiveBatchSpawnerCount = 0;
	WaveInfo.SpawnedActorsDestroyedCount = 0;
	WaveInfo.BatchSpawnData = WaveSpawnData->BatchSpawnData;

	if (const float WaveCountdown = WaveSpawnData->WaveDelay; WaveCountdown > 0)
	{
		OnBeginWaveCountdown.Broadcast(this, WaveCountdown);
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &AWaveSpawnerController::BeginWaveSpawning), WaveCountdown, false);
	}
	else
	{
		BeginWaveSpawning();
	}
}

void AWaveSpawnerController::BeginWaveSpawning_Implementation()
{
	OnBeginWave.Broadcast(this);

	if (!WaveInfo.BatchSpawnData.Num())
	{
		EndWave();
		return;
	}
	
	for (const FBatchSpawnData& BatchSpawnData : WaveInfo.BatchSpawnData)
	{
		if (!SpawnPoints.Contains(BatchSpawnData.SpawnPointID))
		{
			continue;
		}

		AWaveSpawnPoint* WaveSpawnPoint = SpawnPoints[BatchSpawnData.SpawnPointID];
		if (!WaveSpawnPoint)
		{
			continue;
		}
		
		UBatchSpawnLatentAction* AsyncAction = UBatchSpawnLatentAction::BatchSpawnAsyncAction(GetWorld(), BatchSpawnData, WaveSpawnPoint);
		AsyncAction->OnActorSpawned.AddDynamic(this, &AWaveSpawnerController::OnActorSpawned);
		AsyncAction->OnBatchComplete.AddDynamic(this, &AWaveSpawnerController::OnBatchComplete);
		AsyncAction->Activate();
		WaveInfo.ActiveBatchSpawnerCount++;
	}
}

void AWaveSpawnerController::OnActorSpawned_Implementation(AActor* SpawnedActor)
{
	if (!SpawnedActor)
	{
		return;
	}
	SpawnedActor->OnDestroyed.AddDynamic(this, &AWaveSpawnerController::OnActorDestroyed);
	SpawnedActors.Add(SpawnedActor);
}

void AWaveSpawnerController::OnBatchComplete_Implementation()
{
	WaveInfo.ActiveBatchSpawnerCount--;
}

void AWaveSpawnerController::OnActorDestroyed_Implementation(AActor* DestroyedActor)
{
	SpawnedActors.Remove(DestroyedActor);
	WaveInfo.SpawnedActorsDestroyedCount++;
	if (!WaveInfo.ActiveBatchSpawnerCount && !SpawnedActors.Num())
	{
		EndWave();
	}
}

void AWaveSpawnerController::EndWave_Implementation()
{
	OnEndWave.Broadcast(this);
	if (WaveInfo.CurrentWave < CachedTableRows.Num() - 1)
	{
		BeginWave();
	}
	else
	{
		OnCompletedWaves.Broadcast(this);
	}
}
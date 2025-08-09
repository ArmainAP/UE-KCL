// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "WaveSpawner/WaveSpawnerController.h"
#include "Logging.h"
#include "WaveSpawningSystemSettings.h"
#include "Data/BatchSpawnData.h"
#include "Data/WaveSpawnData.h"
#include "WaveSpawner/WaveSpawnHandler.h"

// Called when the game starts or when spawned
void AWaveSpawnerController::BeginPlay()
{
	Super::BeginPlay();
		
	if (bActive)
	{
		ActivateWaves(WaveDataTable);
	}
}

void AWaveSpawnerController::ActivateWaves_Implementation(UDataTable* DataTable)
{
	WaveDataTable = DataTable;
	if (WaveDataTable)
	{
		CachedTableRows = WaveDataTable->GetRowNames();
		OnBeginWaves.Broadcast(this);
		BeginWave();
		bActive = true;
	}
	else
	{
		UE_LOG(WaveSpawningSystemLog, Error, TEXT("WaveSpawnerController::ActivateWaves - WaveDataTable is NULL"));
	}
}

int AWaveSpawnerController::GetWaveActorCount()
{
	return WaveInfo.CurrentWaveActorCount;
}

void AWaveSpawnerController::Activate()
{
	ActivateWaves(WaveDataTable);
}

AActor* AWaveSpawnerController::GetSpawnPoint_Implementation()
{
	if (AActor** ActorPtr = SpawnPoints.Find(CurrentSpawnPointID))
	{
		if (AActor* Actor = *ActorPtr)
		{
			return Actor;
		}
	}
	return Super::GetSpawnPoint_Implementation();
}

void AWaveSpawnerController::Deactivate_Implementation(const bool bEndCurrentWave)
{
	bActive = false;
	if (bEndCurrentWave)
	{
		for (UWaveSpawnHandler* Spawner : WaveInfo.ActiveBatchSpawners)
		{
			Spawner->Cancel();
		}
		
		EndWave();
	}
}

void AWaveSpawnerController::OnBatchComplete_Implementation(UWaveSpawnHandler* BatchSpawner)
{
	WaveInfo.ActiveBatchSpawners.Remove(BatchSpawner);
}

void AWaveSpawnerController::StartTimers()
{
	OnWaveCountdown.Broadcast(this, WaveInfo.WaveCountdown);
		
	GetWorld()->GetTimerManager().SetTimer(OngoingTimerHandle, FTimerDelegate::CreateLambda([&, this]()
	{
		WaveInfo.WaveCountdown -= CountdownUpdateFrequency;
		OnWaveCountdown.Broadcast(this, WaveInfo.WaveCountdown);
	}), CountdownUpdateFrequency, true);
		
	GetWorld()->GetTimerManager().SetTimer(CompletedTimerHandle, FTimerDelegate::CreateUObject(this, &AWaveSpawnerController::BeginWaveSpawning), WaveInfo.WaveCountdown, false);
}

void AWaveSpawnerController::ClearTimers()
{
	if (const UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.ClearTimer(OngoingTimerHandle);
		TimerManager.ClearTimer(CompletedTimerHandle);
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

	WaveInfo.WaveSpawnData = *WaveSpawnData;
	WaveInfo.ActiveBatchSpawners.Reset();
	WaveInfo.SpawnedActorsDestroyedCount = 0;
	WaveInfo.WaveCountdown = WaveSpawnData->WaveDelay;

	if (WaveInfo.WaveCountdown > 0)
	{
		StartTimers();
	}
	else
	{
		BeginWaveSpawning();
	}
}

void AWaveSpawnerController::BeginWaveSpawning_Implementation()
{
	ClearTimers();
	OnBeginWave.Broadcast(this);

	if (!WaveInfo.WaveSpawnData.BatchSpawnData.Num())
	{
		EndWave();
		return;
	}

	WaveInfo.CurrentWaveActorCount = 0;
	for (const FBatchSpawnData& BatchSpawnData : WaveInfo.WaveSpawnData.BatchSpawnData)
	{
		WaveInfo.CurrentWaveActorCount += BatchSpawnData.SpawnCount;

		// TODO: Refactor
		CurrentSpawnPointID = BatchSpawnData.SpawnPointID;
		AActor* WaveSpawnPoint = GetSpawnPoint_Implementation();
		if (!WaveSpawnPoint)
		{
			return;
		}

		UWaveSpawnHandler* WaveSpawnHandler = NewObject<UWaveSpawnHandler>(WaveSpawnPoint, BatchSpawnData.SpawnHandler ? BatchSpawnData.SpawnHandler : GetDefault<UWaveSpawningSystemSettings>()->GetDefaultSpawnHandlerClass());
		WaveSpawnHandler->SetSpawnData(BatchSpawnData);
		WaveSpawnHandler->OnActorSpawned.AddUniqueDynamic(this, &AWaveSpawnerController::OnActorSpawned);
		WaveSpawnHandler->OnBatchComplete.AddUniqueDynamic(this, &AWaveSpawnerController::OnBatchComplete);
		WaveSpawnHandler->Activate();
		WaveInfo.ActiveBatchSpawners.Add(WaveSpawnHandler);
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

void AWaveSpawnerController::OnActorDestroyed_Implementation(AActor* DestroyedActor)
{
	SpawnedActors.Remove(DestroyedActor);
	WaveInfo.SpawnedActorsDestroyedCount++;
	if (!WaveInfo.ActiveBatchSpawners.Num() && !SpawnedActors.Num() && bActive)
	{
		EndWave();
	}
}

void AWaveSpawnerController::EndWave_Implementation()
{
	OnEndWave.Broadcast(this);

	if (!bActive)
	{
		return;
	}

	if (WaveInfo.CurrentWave >= CachedTableRows.Num())
	{
		OnCompletedWaves.Broadcast(this);
		return;
	}

	if (bAutoBeginWave)
	{
		BeginWave();
	}
}

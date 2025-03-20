// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "WaveSpawner/WaveSpawnerController.h"
#include "Logging.h"
#include "Data/BatchSpawnData.h"
#include "Data/WaveSpawnData.h"
#include "Data/WaveSpawnHandlerDataAsset.h"

// Sets default values
AWaveSpawnerController::AWaveSpawnerController()
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

void AWaveSpawnerController::ActivateWaves_Implementation(UDataTable* DataTable)
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

int AWaveSpawnerController::GetWaveActorCount()
{
	int Total = 0;
	for (const TInstancedStruct<FBatchSpawnData>& InstancedBatchSpawnData : WaveInfo.WaveSpawnData.BatchSpawnData)
	{
		if (const FBatchSpawnData* BatchSpawnData = InstancedBatchSpawnData.GetPtr<FBatchSpawnData>())
		{
			Total += BatchSpawnData->SpawnCount;
		}
	}
	return Total;
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
	WaveInfo.ActiveBatchSpawnerCount = 0;
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
	
	for (const TInstancedStruct<FBatchSpawnData>& InstancedBatchSpawnData : WaveInfo.WaveSpawnData.BatchSpawnData)
	{
		FBatchSpawnData BatchSpawnData = InstancedBatchSpawnData.Get<FBatchSpawnData>();
		if (!SpawnPoints.Contains(BatchSpawnData.SpawnPointID))
		{
			continue;
		}

		AWaveSpawnPoint* WaveSpawnPoint = SpawnPoints[BatchSpawnData.SpawnPointID];
		if (!WaveSpawnPoint)
		{
			continue;
		}

		UWaveSpawnHandlerDataAsset* SpawnHandler = IsValid(BatchSpawnData.SpawnHandler) ? BatchSpawnData.SpawnHandler : NewObject<UWaveSpawnHandlerDataAsset>(this);
		SpawnHandler->OnActorSpawned.AddUniqueDynamic(this, &AWaveSpawnerController::OnActorSpawned);
		SpawnHandler->OnBatchComplete.AddUniqueDynamic(this, &AWaveSpawnerController::OnBatchComplete);
		SpawnHandler->BeginSpawn(WaveSpawnPoint, BatchSpawnData);
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
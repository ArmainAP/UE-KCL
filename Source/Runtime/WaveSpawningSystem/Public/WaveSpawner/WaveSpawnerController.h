// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/WaveControllerInfo.h"
#include "GameFramework/Actor.h"
#include "WaveSpawnerController.generated.h"

class UBatchSpawnLatentAction;
class AWaveSpawnPoint;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaveControllerEvent, AWaveSpawnerController*, WaveController);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWaveCountdown, AWaveSpawnerController*, WaveController, float, WaveCountdownLeft);

UCLASS()
class WAVESPAWNINGSYSTEM_API AWaveSpawnerController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaveSpawnerController();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetWaveActorCount();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ActivateWaves(UDataTable* DataTable);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BeginWave();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BeginWaveSpawning();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnActorSpawned(AActor* SpawnedActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnBatchComplete();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnActorDestroyed(AActor* DestroyedActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EndWave();

	void StartTimers();
	void ClearTimers();

public:
	UPROPERTY(BlueprintAssignable)
	FWaveControllerEvent OnBeginWaves;
	
	UPROPERTY(BlueprintAssignable)
	FWaveCountdown OnWaveCountdown;

	UPROPERTY(BlueprintAssignable)
	FWaveControllerEvent OnBeginWave;

	UPROPERTY(BlueprintAssignable)
	FWaveControllerEvent OnEndWave;

	UPROPERTY(BlueprintAssignable)
	FWaveControllerEvent OnCompletedWaves;
	
protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	UDataTable* WaveDataTable = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	TMap<FName, AWaveSpawnPoint*> SpawnPoints;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FWaveControllerInfo WaveInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FName> CachedTableRows;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<AActor*> SpawnedActors;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category=Timer)
	float CountdownUpdateFrequency = 1.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Timer)
	FTimerHandle OngoingTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Timer)
	FTimerHandle CompletedTimerHandle;
};

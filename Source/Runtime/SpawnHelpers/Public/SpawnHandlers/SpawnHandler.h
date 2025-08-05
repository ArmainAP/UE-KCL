// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CancellableAsyncAction.h"
#include "SpawnHandler.generated.h"

// Delegate for when an actor is spawned
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorSpawnedEvent, AActor*, SpawnedActor);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class SPAWNHELPERS_API USpawnHandler : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR
	
	virtual void Activate() override;
	virtual void Cancel() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RequestSpawn();

	UFUNCTION(BlueprintNativeEvent)
	FTransform GetSpawnActorTransform() const;

	UFUNCTION(BlueprintGetter)
	TSubclassOf<AActor> GetActorToSpawnClass() const { return ActorToSpawn; };

protected:
	UFUNCTION(BlueprintNativeEvent)
	void BeginSpawnActor();

	UFUNCTION(BlueprintNativeEvent)
	void SpawnActor(const FTransform& Transform);

	UFUNCTION(BlueprintNativeEvent)
	void PostSpawnActor(AActor* Actor);
	virtual void PostSpawnActor_Implementation(AActor* Actor) {}

	void ClearTimers();

public:
	/** A delegate called when an actor is spawned. */
	UPROPERTY(BlueprintAssignable)
	FActorSpawnedEvent OnActorSpawned;

protected:
	UPROPERTY()
	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FirstSpawnDelay = -1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnInterval = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SpawnCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintGetter = GetActorToSpawnClass)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESpawnActorCollisionHandlingMethod SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
};

// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "ContinuousActorSpawner.generated.h"

/**
 * 
 */
UCLASS()
class SPAWNHELPERS_API AContinuousActorSpawner : public ATargetPoint
{
	GENERATED_BODY()
	
public:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

	AContinuousActorSpawner(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintGetter)
	TSubclassOf<AActor> GetActorToSpawnClass() const { return ActorToSpawn; };

	UFUNCTION(BlueprintPure, BlueprintNativeEvent)
	FTransform GetNextSpawnTransform() const;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void Spawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintGetter = GetActorToSpawnClass)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SpawnOnStart = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnInterval = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESpawnActorCollisionHandlingMethod SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	UPROPERTY()
	FTimerHandle TimerHandle;
};

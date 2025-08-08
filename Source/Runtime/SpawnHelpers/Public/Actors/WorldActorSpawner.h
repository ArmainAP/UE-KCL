// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "SpawnHandlers/SpawnHandler.h"
#include "WorldActorSpawner.generated.h"

/**
 * 
 */
UCLASS()
class SPAWNHELPERS_API AWorldActorSpawner : public ATargetPoint
{
	GENERATED_BODY()
	
public:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

	AWorldActorSpawner(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintGetter)
	USpawnHandler* GetSpawnHandler() const { return SpawnHandler; }
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, Instanced, BlueprintGetter=GetSpawnHandler)
	TObjectPtr<USpawnHandler> SpawnHandler;

	UPROPERTY(EditAnywhere)
	bool bAutoActivate = true;
};

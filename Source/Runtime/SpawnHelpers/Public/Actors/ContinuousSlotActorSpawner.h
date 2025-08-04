// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ContinuousActorSpawner.h"
#include "ContinuousSlotActorSpawner.generated.h"

class UFormationGroupComponent;

/**
 * 
 */
UCLASS()
class SPAWNHELPERS_API AContinuousSlotActorSpawner : public AContinuousActorSpawner
{
	GENERATED_BODY()
	
public:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR
	
	AContinuousSlotActorSpawner(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintGetter)
	UFormationGroupComponent* GetFormationGroupComponent() const { return FormationGroupComponent; }

protected:
	virtual FTransform GetNextSpawnTransform_Implementation() const override;
	virtual void Spawn_Implementation() override;
	
	UPROPERTY(EditAnywhere, BlueprintGetter=GetFormationGroupComponent, Category=Components)
	TObjectPtr<UFormationGroupComponent> FormationGroupComponent;
};

// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SpawnHandlers/SpawnHandler.h"
#include "FormationSlotSpawnHandler.generated.h"

class UFormationGroupComponent;

/**
 * 
 */
UCLASS()
class SPAWNHELPERS_API UFormationSlotSpawnHandler : public USpawnHandler
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

	virtual void Activate() override;
	
	UFUNCTION(BlueprintSetter)
	void SetFormationGroupComponent(UFormationGroupComponent* InFormationGroupComponent)
	{
		FormationGroupComponent = InFormationGroupComponent;
	}
	
protected:
	virtual FTransform GetSpawnActorTransform_Implementation() const override;
	virtual void BeginSpawnActor_Implementation() override;
	
	UPROPERTY(BlueprintSetter=SetFormationGroupComponent)
	TObjectPtr<UFormationGroupComponent> FormationGroupComponent;
};

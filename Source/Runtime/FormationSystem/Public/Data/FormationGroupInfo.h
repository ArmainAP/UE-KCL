// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/FormationComponent.h"
#include "FormationGroupInfo.generated.h"

class UFormationComponent;
class UFormationDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFormationGroupEvent);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class FORMATIONSYSTEM_API UFormationGroupInfo : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Setup(const TArray<UFormationComponent*>& InUnits);
	
	UFUNCTION(BlueprintCallable)
	void AddController(UFormationComponent* Unit);

	UFUNCTION(BlueprintCallable)
	void RemoveController(UFormationComponent* Unit);

	UFUNCTION(BlueprintCallable)
	void ChangeFormation(UFormationDataAsset* InFormationDataAsset);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HaveAllUnitsReached();

	UFUNCTION(BlueprintCallable)
	void StopMovement();

	UFUNCTION(BlueprintCallable)
	void MoveFormation(const FVector Location, const FVector Direction);

protected:
	UFUNCTION()
	void OnUnitReached(UFormationComponent* Unit);

	UFUNCTION()
	void OnOwnerDestroyed(AActor* DestroyedActor);

public:
	UPROPERTY(BlueprintAssignable)
	FFormationGroupEvent OnAllUnitsReached;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UFormationComponent*> Units;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTransform TargetTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UFormationDataAsset* FormationDataAsset;
};

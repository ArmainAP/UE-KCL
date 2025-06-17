// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/FormationHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "FormationSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FFormationHandleEvent, const FName GroupID, UFormationComponent*);

/**
 * 
 */
UCLASS()
class FORMATIONSYSTEM_API UFormationSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UFormationSubsystem* Get(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable)
	bool CreateGroup(const FName GroupID, UFormationDataAsset* DataAsset);

	UFUNCTION(BlueprintCallable)
	bool DestroyGroup(FName GroupID);
	
	UFUNCTION(BlueprintCallable)
	bool AddUnit(const FName GroupID, UFormationComponent* FormationUnit);
	
	UFUNCTION(BlueprintCallable)
	bool RemoveUnit(const FName GroupID, UFormationComponent* FormationUnit);

	template<typename OutT>
	bool GetUnits(const FName GroupID, OutT&& Output) const;
	
	UFUNCTION(BlueprintPure)
	bool GetUnitsSet(const FName GroupID, TSet<UFormationComponent*>& Units) const;

	UFUNCTION(BlueprintPure)
	bool GetUnitsArray(FName GroupID, TArray<UFormationComponent*>& Units) const;

	UFUNCTION(BlueprintPure)
	int GetUnitsCount(const FName GroupID) const;
	
	UFUNCTION(BlueprintCallable)
	bool StopMovement(const FName GroupID) const;

	UFUNCTION(BlueprintCallable)
	bool MoveFormation(const FName GroupID, const FVector& Location, const FVector& Direction = FVector::ForwardVector) const;

	UFUNCTION(BlueprintCallable)
	bool SetFormationDataAsset(const FName GroupID, UFormationDataAsset* InFormationDataAsset);

	UFUNCTION(BlueprintPure)
	UFormationDataAsset* GetFormationDataAsset(const FName GroupID) const;

	UFUNCTION(BlueprintPure)
	FVector GetFormationAverageLocation(const FName GroupID) const;

	UFUNCTION(BlueprintPure)
	AActor* GetFormationLead(const FName GroupID) const;
	
	UFUNCTION(BlueprintPure)
	FVector GetFormationLeadLocation(const FName GroupID) const;

	FFormationHandleEvent OnUnitJoined;
	FFormationHandleEvent OnUnitLeft;
	
protected:
	TMap<FName, FFormationHandle> FormationHandles;
};
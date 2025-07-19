// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/FormationHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "FormationSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FFormationHandleEvent, const FName GroupID, UFormationComponent*);
DECLARE_DYNAMIC_DELEGATE_OneParam(FFormationUnitDynDelegate, UFormationComponent*, Unit);

using FFormationUnitCallable = TFunctionRef<void(UFormationComponent*)>;

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
	bool CreateGroup(const FName GroupID, UFormationDataAsset* DataAsset, AActor* FormationOwner = nullptr);

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

	bool ForEachUnit(const FName GroupID, FFormationUnitCallable Callable) const;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "For Each Unit", AutoCreateRefTerm = "BPDelegate"))
	bool ForEachUnitBP(const FName GroupID, const FFormationUnitDynDelegate& BPDelegate) const;

	UFUNCTION(BlueprintPure)
	AActor* GetFormationOwner(const FName GroupID);

	FFormationHandleEvent OnUnitJoined;
	FFormationHandleEvent OnUnitLeft;
	
protected:
	TMap<FName, FFormationHandle> FormationHandles;
};
// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FormationContext.generated.h"

class UFormationContext;
class UFormationDataAsset;
class UFormationComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFormationHandleEvent, UFormationContext*, Context, UFormationComponent*, Unit);
DECLARE_DYNAMIC_DELEGATE_OneParam(FFormationUnitDynDelegate, UFormationComponent*, Unit);
using FFormationUnitCallable = TFunctionRef<void(UFormationComponent*)>;

/**
 * 
 */
UCLASS()
class FORMATIONSYSTEM_API UFormationContext : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool AddUnit(UFormationComponent* FormationUnit);
	
	UFUNCTION(BlueprintCallable)
	bool RemoveUnit(UFormationComponent* FormationUnit);

	void ForEachUnit(const FFormationUnitCallable& Callable) const;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "For Each Unit", AutoCreateRefTerm = "BPDelegate"))
	void ForEachUnitBP(const FFormationUnitDynDelegate& BPDelegate) const;
	
	UFUNCTION(BlueprintSetter)
	void SetFormationDataAsset(UFormationDataAsset* InFormationDataAsset) { FormationDataAsset = InFormationDataAsset; }

	UFUNCTION(BlueprintGetter)
	UFormationDataAsset* GetFormationDataAsset() const { return FormationDataAsset; }

	UFUNCTION(BlueprintSetter)
	void SetFormationOwner(AActor* InFormationOwner) { FormationOwner = InFormationOwner; }
	
	UFUNCTION(BlueprintGetter)
	AActor* GetFormationOwner() const { return FormationOwner.Get(); }

	UFUNCTION(BlueprintPure)
	int GetUnitsCount() const;

	UFUNCTION(BlueprintCallable)
	int CullInvalidUnits() const;
	
	UFUNCTION(BlueprintCallable)
	void RequestMove(const FTransform& WorldTransform) const;

	UFUNCTION(BlueprintPure)
	FVector GetFormationAverageLocation() const;

	UFUNCTION(BlueprintPure)
	AActor* GetFormationLead() const;

	UFUNCTION(BlueprintPure)
	void GetUnitsSet(TSet<UFormationComponent*>& OutUnits) const;

	UFUNCTION(BlueprintPure)
	void GetUnitsArray(TArray<UFormationComponent*>& OutUnits) const;

	UFUNCTION(BlueprintPure)
	bool IsFull() const;

	UFUNCTION(BlueprintPure)
	bool IsEmpty() const;

	UFUNCTION(BlueprintCallable)
	FTransform GetTranformForIndex(const int Index) const;

	UPROPERTY(BlueprintAssignable)
	FFormationHandleEvent OnUnitJoined;

	UPROPERTY(BlueprintAssignable)
	FFormationHandleEvent OnUnitLeft;

protected:
	template<typename OutT>
	void GetUnits(OutT&& Output) const;
	
	mutable TSet<TWeakObjectPtr<UFormationComponent>, TWeakObjectPtrSetKeyFuncs<TWeakObjectPtr<UFormationComponent>>> Units;

	UPROPERTY(VisibleAnywhere, BlueprintSetter=SetFormationDataAsset, BlueprintGetter=GetFormationDataAsset)
	TObjectPtr<UFormationDataAsset> FormationDataAsset = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintSetter=SetFormationOwner, BlueprintGetter=GetFormationOwner)
	TObjectPtr<AActor> FormationOwner = nullptr;
};

// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FormationGroupInfo.generated.h"

class IFormationUnit;
class UFormationDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitsChanged, TScriptInterface<IFormationUnit>, Unit);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class FORMATIONSYSTEM_API UFormationGroupInfo : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool AddUnit(const TScriptInterface<IFormationUnit>& Unit);

	UFUNCTION(BlueprintCallable)
	bool RemoveUnit(const TScriptInterface<IFormationUnit>& Unit);

	UFUNCTION(BlueprintCallable)
	void StopMovement();

	UFUNCTION(BlueprintCallable)
	void MoveFormation(const FVector& Location, const FVector& Direction = FVector::ForwardVector);

	UFUNCTION(BlueprintCallable)
	void SetFormationDataAsset(UFormationDataAsset* InFormationDataAsset);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetUnitsCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<TScriptInterface<IFormationUnit>> GetUnits();

	UFUNCTION(BlueprintCallable)
	UFormationDataAsset* GetFormationDataAsset() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetFormationAverageLocation() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetFormationLead();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetFormationLeadLocation();
	
	UPROPERTY(BlueprintAssignable)
	FUnitsChanged OnFormationUnitLeft;

	UPROPERTY(BlueprintAssignable)
	FUnitsChanged OnFormationUnitJoined;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintGetter=GetUnits)
	TArray<TScriptInterface<IFormationUnit>> Units;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintGetter=GetFormationDataAsset, BlueprintSetter=SetFormationDataAsset)
	UFormationDataAsset* FormationDataAsset;
};

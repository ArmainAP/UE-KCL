// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FormationDataAsset.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class FORMATIONSYSTEM_API UFormationDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	void GetOffsetTransforms(const int UnitCount, TArray<FTransform>& OutTransforms) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	FTransform GetOffsetTransformForIndex(int Index, const int UnitCount) const;

	UFUNCTION(BlueprintGetter)
	int GetFormationLimit() const { return FormationLimit; }

	UFUNCTION(BlueprintPure)
	int GetCappedUnitCount(const int UnitCount) const; 
	
protected:
	UPROPERTY(EditAnywhere, BlueprintGetter=GetFormationLimit)
	int FormationLimit = INDEX_NONE;
};

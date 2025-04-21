// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FormationDataAsset.generated.h"

class IFormationUnit;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class FORMATIONSYSTEM_API UFormationDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	void GetOffsetTransforms(const int UnitCount, TArray<FTransform>& OutTransforms);

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	void GetWorldTransforms(const int UnitCount, const FVector& Location, const FVector& Direction, TArray<FTransform>& OutTransforms);
};

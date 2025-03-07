// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FormationDataAsset.generated.h"

class UFormationComponent;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class FORMATIONSYSTEM_API UFormationDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static void ExtractTransforms(const TArray<UFormationComponent*>& Units, TArray<FTransform>& OutTransforms);

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	void GetOffsetTransforms(const TArray<UFormationComponent*>& Units, TArray<FTransform>& OutTransforms);

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	void GetWorldTransforms(const TArray<UFormationComponent*>& Units, const FVector& Location, const FVector& Direction, TArray<FTransform>& OutTransforms);
};

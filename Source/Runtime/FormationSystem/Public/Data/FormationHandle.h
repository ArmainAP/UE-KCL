// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/FormationComponent.h"
#include "FormationDataAssets/FormationDataAsset.h"
#include "FormationHandle.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FFormationHandle
{
	GENERATED_BODY()
	
	TSet<TWeakObjectPtr<UFormationComponent>, TWeakObjectPtrSetKeyFuncs<TWeakObjectPtr<UFormationComponent>>> Units;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UFormationDataAsset> FormationDataAsset = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TWeakObjectPtr<AActor> FormationOwner = nullptr;
};

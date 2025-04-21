// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/FormationComponent.h"
#include "FormationDataAssets/FormationDataAsset.h"
#include "FormationHandle.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FFormationHandleEvent, UFormationComponent*);

USTRUCT(BlueprintType, Blueprintable)
struct FFormationHandle
{
	GENERATED_BODY()
	
	TSet<TWeakObjectPtr<UFormationComponent>> Units;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UFormationDataAsset> FormationDataAsset = nullptr;

	FFormationHandleEvent OnUnitJoined;
	FFormationHandleEvent OnUnitLeft;
};

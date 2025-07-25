// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "TaggedAssetBundleDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class TAGGEDASSETS_API UTaggedAssetBundleDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintGetter)
	const TMap<FGameplayTag, TSoftObjectPtr<UObject>>& GetTagAssetMap() const { return TagAssetMap; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintGetter=GetTagAssetMap)
	TMap<FGameplayTag, TSoftObjectPtr<UObject>> TagAssetMap;
};

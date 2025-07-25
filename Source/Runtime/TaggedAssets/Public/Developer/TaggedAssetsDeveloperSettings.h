// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TaggedAssetsDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Engine, DefaultConfig)
class TAGGEDASSETS_API UTaggedAssetsDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UTaggedAssetsDeveloperSettings()
	{
		CategoryName = "KCL";
		SectionName = "Tagged Assets Developer Settings";
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, meta=(AllowedClasses="TaggedAssetBundleDataAsset"))
	TSet<FSoftObjectPath> Bundles;
};

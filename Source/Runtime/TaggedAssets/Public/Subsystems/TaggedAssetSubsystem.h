// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TaggedAssetSubsystem.generated.h"

class UTaggedAssetBundleDataAsset;
/**
 * 
 */
UCLASS()
class TAGGEDASSETS_API UTaggedAssetSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION(BlueprintPure)
	static UTaggedAssetSubsystem* Get();
	
	UFUNCTION(BlueprintCallable)
	bool RegisterAsset(const FGameplayTag& AssetTag, UObject* Asset);

	UFUNCTION(BlueprintCallable)
	bool RegisterBundle(const UTaggedAssetBundleDataAsset* Bundle);

	UFUNCTION(BlueprintPure, meta=(DeterminesOutputType="Class"))
	UObject* GetAsset(const FGameplayTag& AssetTag, const UClass* Class = nullptr) const;

	template<class T>
	T* GetAsset(const FGameplayTag& AssetTag) const
	{
		return Cast<T>(GetAsset(AssetTag));
	}

protected:
	void OnBundleLoaded(const FSoftObjectPath& SoftObjectPath, UObject* Object);
	
	static UTaggedAssetSubsystem* StaticThis;

	UPROPERTY()
	TMap<FGameplayTag, TSoftObjectPtr<UObject>> TagAssetMap;

	UPROPERTY()
	TSet<TObjectPtr<const UTaggedAssetBundleDataAsset>> Bundles;
};

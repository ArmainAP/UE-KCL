// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Subsystems/TaggedAssetSubsystem.h"

#include "Data/TaggedAssetBundleDataAsset.h"
#include "Developer/TaggedAssetsDeveloperSettings.h"

UTaggedAssetSubsystem* UTaggedAssetSubsystem::StaticThis = nullptr;

void UTaggedAssetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	StaticThis = this;

	const UTaggedAssetsDeveloperSettings* TaggedAssetsDeveloperSettings = GetDefault<UTaggedAssetsDeveloperSettings>();
	check(TaggedAssetsDeveloperSettings);
	for (const FSoftObjectPath& ObjectPath : TaggedAssetsDeveloperSettings->Bundles)
	{
		ObjectPath.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateUObject(this, &UTaggedAssetSubsystem::OnBundleLoaded));
	}
}

UTaggedAssetSubsystem* UTaggedAssetSubsystem::Get()
{
	return StaticThis;
}

bool UTaggedAssetSubsystem::RegisterAsset(const FGameplayTag& AssetTag, UObject* Asset)
{
	if (AssetTag.IsValid() && Asset)
	{
		ensureAlwaysMsgf(!TagAssetMap.Contains(AssetTag), TEXT("%s was overriden with %s"), *AssetTag.ToString(), *Asset->GetPathName());
		TagAssetMap.Emplace(AssetTag, Asset);
		return true;
	}
	return false;
}

bool UTaggedAssetSubsystem::RegisterBundle(const UTaggedAssetBundleDataAsset* Bundle)
{
	if (!Bundle)
	{
		return false;
	}

	const TMap<FGameplayTag, TSoftObjectPtr<UObject>>& BundledTagAssetMap = Bundle->GetTagAssetMap();
	if (BundledTagAssetMap.IsEmpty())
	{
		return false;
	}
	
	bool bIsAlreadyRegistered = false;
	Bundles.Add(Bundle, &bIsAlreadyRegistered);
	if (!ensureAlwaysMsgf(!bIsAlreadyRegistered, TEXT("%s is already registered."), *Bundle->GetPathName()))
	{
		return false;
	}

	for (auto Pair : BundledTagAssetMap)
	{
		RegisterAsset(Pair.Key, Pair.Value.Get());
	}

	return true;
}

UObject* UTaggedAssetSubsystem::GetAsset(const FGameplayTag& AssetTag, const UClass* Class) const
{
	const auto FoundPtr = TagAssetMap.Find(AssetTag);
	return FoundPtr ? FoundPtr->Get() : nullptr;
}

void UTaggedAssetSubsystem::OnBundleLoaded(const FSoftObjectPath& SoftObjectPath, UObject* Object)
{
	RegisterBundle(Cast<UTaggedAssetBundleDataAsset>(Object));
}

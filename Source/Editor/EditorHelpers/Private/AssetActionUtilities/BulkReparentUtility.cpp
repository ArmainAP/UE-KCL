// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActionUtilities/BulkReparentUtility.h"

#include "BlueprintEditorLibrary.h"
#include "EditorAssetLibrary.h"
#include "EditorUtilityLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UBulkReparentUtility::UBulkReparentUtility()
{
	SupportedClasses = { UBlueprint::StaticClass() };
}

void UBulkReparentUtility::ExecuteReparent(const TSoftClassPtr<UObject> FromClass, const TSoftClassPtr<UObject> ToClass) const
{
	for (UObject* Asset : UEditorUtilityLibrary::GetSelectedAssetsOfClass(UBlueprint::StaticClass()))
	{
		UKismetSystemLibrary::CreateCopyForUndoBuffer(Asset);

		UBlueprint* Blueprint = UBlueprintEditorLibrary::GetBlueprintAsset(Asset);
		if (!Blueprint)
		{
			continue;
		}

		if (Blueprint->ParentClass && Blueprint->ParentClass->IsChildOf(FromClass.Get()))
		{
			UEditorAssetLibrary::CheckoutLoadedAsset(Asset);
			UBlueprintEditorLibrary::ReparentBlueprint(Blueprint, ToClass.Get());
		}
	}
}

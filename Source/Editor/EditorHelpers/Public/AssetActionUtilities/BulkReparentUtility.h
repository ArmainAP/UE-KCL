// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Editor/Blutility/Classes/AssetActionUtility.h"
#include "BulkReparentUtility.generated.h"

/**
 * 
 */
UCLASS()
class EDITORHELPERS_API UBulkReparentUtility : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UBulkReparentUtility();

protected:
	UFUNCTION(BlueprintCallable)
	void ExecuteReparent(TSoftClassPtr<UObject> FromClass, TSoftClassPtr<UObject> ToClass) const;
};

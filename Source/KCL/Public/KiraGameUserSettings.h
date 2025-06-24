// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "KiraGameUserSettings.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FKiraGameUserSettingsChanged);

/**
 * Do not forget to add the following to DefaultEngine.ini:
 * 
 * [/Script/Engine.Engine]
 * GameUserSettingsClassName=/Script/KCL.KiraGameUserSettings
 * 
 */
UCLASS()
class KCL_API UKiraGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static UKiraGameUserSettings* Get();

	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;

	UPROPERTY(BlueprintAssignable)
	FKiraGameUserSettingsChanged OnKiraGameUserSettingsChanged;

protected:
	UPROPERTY(Config, BlueprintGetter=GetFieldOfView, BlueprintSetter=SetFieldOfView)
	float FieldOfView = 90.0f;

public:
	UFUNCTION(BlueprintGetter)
	float GetFieldOfView() const { return FieldOfView; }
	
	UFUNCTION(BlueprintSetter)
	void SetFieldOfView(const float& InFieldOfView) { FieldOfView = InFieldOfView; }
};
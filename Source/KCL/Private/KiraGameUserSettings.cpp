// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "KiraGameUserSettings.h"

UKiraGameUserSettings* UKiraGameUserSettings::Get()
{
	return GEngine ? Cast<UKiraGameUserSettings>(GEngine->GetGameUserSettings()) : nullptr;
}

void UKiraGameUserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
	Super::ApplySettings(bCheckForCommandLineOverrides);
	OnKiraGameUserSettingsChanged.Broadcast();
}
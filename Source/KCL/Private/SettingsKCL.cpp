// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "SettingsKCL.h"

#include "Misc/ConfigUtilities.h"

void USettingsKCL::PostInitProperties()
{
#if WITH_EDITOR
	if (IsTemplate())
	{
		// We want the .ini file to have precedence over the CVar constructor, so we apply the ini to the CVar before following the regular UDeveloperSettingsBackedByCVars flow
		UE::ConfigUtilities::ApplyCVarSettingsFromIni(TEXT("/Script/KCL.SettingsKCL"), *GGameIni, ECVF_SetByProjectSetting);
	}
#endif

	Super::PostInitProperties();
}

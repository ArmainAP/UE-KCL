// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "SettingsKCL.generated.h"

/**
 * Settings for Kira's Component Library.
 * These settings are both configurable in the Project Settings and adjustable at runtime via CVars.
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Kira's Component Library"))
class KCL_API USettingsKCL : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;
	
	/** 
	* Toggle to enable or disable the Formation Manager Subsystem.
	* This value can be adjusted in Project Settings and at runtime using the console variable.
	*/
	UPROPERTY(EditAnywhere, Config, Category = "Subsystems", meta = (ConsoleVariable = "KCL.EnableFormationManagerSubsystem"))
	bool bEnableFormationManagerSubsystem = false;


};


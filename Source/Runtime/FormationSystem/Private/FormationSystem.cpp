// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "FormationSystem.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FFormationSystem"

void FFormationSystem::StartupModule()
{
}

void FFormationSystem::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFormationSystem, FormationSystem);
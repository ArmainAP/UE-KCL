// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "SpawnHelpers.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FSpawnHelpers"

void FSpawnHelpers::StartupModule()
{
}

void FSpawnHelpers::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSpawnHelpers, SpawnHelpers);
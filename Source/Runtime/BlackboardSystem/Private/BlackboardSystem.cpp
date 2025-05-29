// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "BlackboardSystem.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FBlackboardSystem"

void FBlackboardSystem::StartupModule()
{
}

void FBlackboardSystem::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBlackboardSystem, BlackboardSystem);
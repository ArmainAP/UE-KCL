// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "HierarchicalStateMachine.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FHierarchicalStateMachine"

void FHierarchicalStateMachine::StartupModule()
{
}

void FHierarchicalStateMachine::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHierarchicalStateMachine, HierarchicalStateMachine);
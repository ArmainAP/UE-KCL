// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "GroundPawnMovement.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FGroundPawnMovement"

void FGroundPawnMovement::StartupModule()
{
}

void FGroundPawnMovement::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGroundPawnMovement, GroundPawnMovement);
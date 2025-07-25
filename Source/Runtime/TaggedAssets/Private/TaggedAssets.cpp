// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "TaggedAssets.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FTaggedAssets"

void FTaggedAssets::StartupModule()
{
}

void FTaggedAssets::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTaggedAssets, TaggedAssets);
// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "BlueprintGameInstanceSubsystem.h"

void UBlueprintGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnInitialize();
}

void UBlueprintGameInstanceSubsystem::Deinitialize()
{
	OnDeinitialize();
	
	Super::Deinitialize();
}

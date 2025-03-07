// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "BlueprintWorldSubsystem.h"

void UBlueprintWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnInitialize();
}

void UBlueprintWorldSubsystem::Deinitialize()
{
	OnDeinitialize();
	
	Super::Deinitialize();
}

void UBlueprintWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();

	OnPostInitialize();
}

void UBlueprintWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	OnWorldBeginPlayEvent(&InWorld);
}

void UBlueprintWorldSubsystem::OnWorldComponentsUpdated(UWorld& World)
{
	Super::OnWorldComponentsUpdated(World);

	OnWorldComponentsUpdatedEvent(&World);
}

bool UBlueprintWorldSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}
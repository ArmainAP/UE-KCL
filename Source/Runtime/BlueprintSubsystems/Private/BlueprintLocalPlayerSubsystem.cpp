// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "BlueprintLocalPlayerSubsystem.h"

void UBlueprintLocalPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnInitialize();
}

void UBlueprintLocalPlayerSubsystem::Deinitialize()
{
	OnDeinitialize();
	
	Super::Deinitialize();
}

void UBlueprintLocalPlayerSubsystem::PlayerControllerChanged(APlayerController* NewPlayerController)
{
	Super::PlayerControllerChanged(NewPlayerController);

	OnPlayerControllerChanged(NewPlayerController);
}

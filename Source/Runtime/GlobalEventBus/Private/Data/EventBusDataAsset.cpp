// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Data/EventBusDataAsset.h"

UEventBusDataAsset::UEventBusDataAsset()
{
	ID = FGuid::NewGuid();
}

void UEventBusDataAsset::StartExecution_Implementation(AActor* InExecutor)
{
	OnTaskStarted.Broadcast(this, InExecutor);
}

void UEventBusDataAsset::Execute_Implementation(AActor* InExecutor)
{
	OnTaskFinished.Broadcast(this, InExecutor);
}

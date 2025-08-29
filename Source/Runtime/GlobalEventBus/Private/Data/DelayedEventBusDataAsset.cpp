// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Data/DelayedEventBusDataAsset.h"

void UDelayedEventBusDataAsset::StartExecution_Implementation(AActor* InExecutor)
{
	Super::StartExecution_Implementation(InExecutor);

	if (ExecutionDelay > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, InExecutor]()
		{
			Execute(InExecutor);
		}, ExecutionDelay, false);
	}
	else
	{
		Execute(InExecutor);
	}
}

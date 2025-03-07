// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "BlueprintTickableWorldSubsystem.h"

void UBlueprintTickableWorldSubsystem::Tick(float DeltaTime)
{
	if (LastFrameNumberWeTicked == GFrameCounter)
	{
		return;	
	}

	OnTick(DeltaTime);

	LastFrameNumberWeTicked = GFrameCounter;
}
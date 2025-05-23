#pragma once 

#include "Components/SightedComponent.h"
#include "Components/SighterComponent.h"
#include "SightQueryContext.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct SIGHTSYSTEM_API FSightQueryContext
{
	GENERATED_BODY()
	
	FSightQueryContext() = default;
	FSightQueryContext(USighterComponent* InSighter, USightedComponent* InSighted)
	{
		Sighter = InSighter;
		Sighted = InSighted;
	};
	
	TWeakObjectPtr<USighterComponent> Sighter;
	TWeakObjectPtr<USightedComponent> Sighted;

	bool bPreviousCheckSucceeded = false;
	bool bCurrenCheckSucceeded = false;
	bool bIsPerceived = false;
	bool bIsVisible = false;

	float TimeSinceLostSight = 0.0f;
	
	float GainValue = 0.0f;
	float GainRate = 1.0f;
	float WaitTime = 0.0f;

	FVector LastSeenLocation = FVector::ZeroVector;
};

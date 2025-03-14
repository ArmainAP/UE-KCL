// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/LineTraceMultiComponent.h"

bool ULineTraceMultiComponent::PerformTrace_Implementation(TArray<FHitResult>& OutHitResults)
{
	const FVector ComponentLocation = GetComponentLocation();
	return UKismetSystemLibrary::LineTraceMulti(GetWorld(), ComponentLocation + StartOffset, ComponentLocation + EndOffset,
	                                     TraceChannel, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHitResults,
	                                     bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
}

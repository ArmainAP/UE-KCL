// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/SphereTraceMultiComponent.h"

bool USphereTraceMultiComponent::PerformTrace_Implementation(TArray<FHitResult>& OutHitResults)
{
	const FVector ComponentLocation = GetComponentLocation();
	return UKismetSystemLibrary::SphereTraceMulti(GetWorld(), ComponentLocation + StartOffset,
	                                              ComponentLocation + EndOffset, Radius, UEngineTypes::ConvertToTraceType(TraceChannel), bTraceComplex,
	                                              ActorsToIgnore, DrawDebugType, OutHitResults, bIgnoreSelf, TraceColor,
	                                              TraceHitColor, DrawTime);
}

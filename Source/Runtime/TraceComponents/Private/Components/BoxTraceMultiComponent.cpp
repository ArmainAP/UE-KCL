// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/BoxTraceMultiComponent.h"

bool UBoxTraceMultiComponent::PerformTrace_Implementation(TArray<FHitResult>& OutHitResults)
{
	const FVector ComponentLocation = GetComponentLocation();
	return UKismetSystemLibrary::BoxTraceMulti(GetWorld(), ComponentLocation + StartOffset, ComponentLocation + EndOffset,
	                                    HalfSize, GetComponentRotation(), UEngineTypes::ConvertToTraceType(TraceChannel), bTraceComplex, ActorsToIgnore,
	                                    DrawDebugType, OutHitResults, bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
}

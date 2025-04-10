// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/CapsuleTraceMultiComponent.h"

bool UCapsuleTraceMultiComponent::PerformTrace_Implementation(TArray<FHitResult>& OutHitResults)
{
	const FVector ComponentLocation = GetComponentLocation();
	return UKismetSystemLibrary::CapsuleTraceMulti(GetWorld(), ComponentLocation + StartOffset, ComponentLocation + EndOffset,
	                                        Radius, HalfHeight, UEngineTypes::ConvertToTraceType(TraceChannel), bTraceComplex, ActorsToIgnore,
	                                        DrawDebugType, OutHitResults, bIgnoreSelf, TraceColor, TraceHitColor,
	                                        DrawTime);
}

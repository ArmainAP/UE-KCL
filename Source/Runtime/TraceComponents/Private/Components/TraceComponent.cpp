// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/TraceComponent.h"

UTraceComponent::UTraceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, 
									FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	NewActors.Reset();
	NewHitResults.Reset();
	
	// 1) Do the trace and get fresh results
	PerformTrace(NewHitResults);

	// Make a copy of all existing keys before we update CurrentHitMap
	TArray<AActor*> OldActors;
	CurrentHitMap.GetKeys(OldActors);

	// 2) Handle newly-hitting actors
	for (const FHitResult& HitResult : NewHitResults)
	{
		AActor* HitActor = HitResult.GetActor();

		// If the actor wasn't in our map last frame, broadcast "Begin"
		if (!CurrentHitMap.Contains(HitActor))
		{
			OnActorTracedBegin.Broadcast(HitResult);
		}

		// Update or add the actor's latest HitResult
		CurrentHitMap.Emplace(HitActor, HitResult);

		// Keep track that this actor was hit in this frame
		NewActors.Add(HitActor);
	}

	// 3) Handle actors no longer being hit (TraceEnd)
	for (const AActor* OldActor : OldActors)
	{
		// If our old actor is not in the new set, it "exited" the trace
		if (OldActor && !NewActors.Contains(OldActor))
		{
			// Broadcast OnActorTracedEnd with the old info
			if (const FHitResult* OldHitResult = CurrentHitMap.Find(OldActor))
			{
				OnActorTracedEnd.Broadcast(*OldHitResult);
			}
			
			// Remove them from the map since they aren't traced this frame
			CurrentHitMap.Remove(OldActor);
		}
	}
}

void UTraceComponent::GetCurrentHitResults(TArray<FHitResult>& OutArray) const
{
	CurrentHitMap.GenerateValueArray(OutArray);
}

bool UTraceComponent::PerformTrace_Implementation(TArray<FHitResult>& OutHitResults)
{
	return false;
}

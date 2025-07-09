// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/TraceComponent.h"

#include "Interfaces/TracedActorNotify.h"

UTraceComponent::UTraceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1;
}

void UTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, 
									FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Increment our "frame" or "tick" counter each time
    ++CurrentFrameID;

    // (A) Perform the trace
    TraceResultsScratch.Reset();
    TraceResultsScratch.Reserve(SizePreAllocation);
    PerformTrace(TraceResultsScratch);

    // (B) Update or add new hits -- only 1 loop over new hits
    for (const FHitResult& Hit : TraceResultsScratch)
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor)
        {
            continue;
        }

        // Find or add
        if (FTracedActorInfo* Found = CurrentHitMap.Find(HitActor))
        {
            // Already in map, just update
            Found->HitResult       = Hit;
            Found->LastFrameTraced = CurrentFrameID;
        }
        else
        {
            // This is a new actor that wasn't previously traced
            FTracedActorInfo NewInfo;
            NewInfo.HitResult       = Hit;
            NewInfo.LastFrameTraced = CurrentFrameID;

            CurrentHitMap.Add(HitActor, NewInfo);

            HandleActorTraceBegin(Hit);
        }
    }

    // (C) Remove old entries AND build CachedHitResults in ONE pass
    CachedHitResults.Reset();
    CachedHitResults.Reserve(CurrentHitMap.Num());

    // Single pass over entire map
    for (auto It = CurrentHitMap.CreateIterator(); It; ++It)
    {
        if (It->Value.LastFrameTraced != CurrentFrameID)
        {
            // This actor wasn't updated this tick; so it just ended
            HandleActorTraceEnd(It->Value.HitResult);
            It.RemoveCurrent();
            continue;
        }

        // Otherwise, add the valid hit to the cached array
        CachedHitResults.Add(It->Value.HitResult);
    }
}

void UTraceComponent::GetCurrentHitResults(TArray<FHitResult>& OutArray) const
{
    OutArray = CachedHitResults;
}

void UTraceComponent::HandleActorTraceBegin_Implementation(const FHitResult& HitResult)
{
    OnActorTracedBegin.Broadcast(HitResult);
    
    if (!TraceTag.IsValid())
    {
        return;
    }

    for (AActor* It = HitResult.GetActor(); IsValid(It); It = It->GetAttachParentActor())
    {
        if (It->Implements<UTracedActorNotify>())
        {
            ITracedActorNotify::Execute_TraceStart(It, this, HitResult);
            break;
        }
    }
}

void UTraceComponent::HandleActorTraceEnd_Implementation(const FHitResult& HitResult)
{
    OnActorTracedEnd.Broadcast(HitResult);
    if (!TraceTag.IsValid())
    {
        return;
    }
    
    for (AActor* It = HitResult.GetActor(); IsValid(It); It = It->GetAttachParentActor())
    {
        if (It->Implements<UTracedActorNotify>())
        {
            ITracedActorNotify::Execute_TraceEnd(It, this, HitResult);
            break;
        }
    }
}

bool UTraceComponent::PerformTrace_Implementation(TArray<FHitResult>& OutHitResults)
{
	return false;
}

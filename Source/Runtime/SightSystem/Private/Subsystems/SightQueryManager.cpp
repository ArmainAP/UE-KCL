// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Subsystems/SightQueryManager.h"

#include "SightSystemFunctionLibrary.h"

void USightQueryManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SightQueries.IsEmpty())
	{
		return;
	}

	FCriticalSection Mutex;
	ParallelFor(SightQueries.Num(), [this, &Mutex, DeltaTime](int32 Idx)
	{
		Mutex.Lock();
		FSightQueryContext& Query = SightQueries[Idx];
		USightSystemFunctionLibrary::EvaluateQueryVisibility(Query);
		ProcessQueryState(Query, DeltaTime);
		Mutex.Unlock();
	});
}

bool USightQueryManager::IsSightedRegistered(const USightedComponent* SightedComponent) const
{
	return SightedComponents.Contains(SightedComponent);
}

void USightQueryManager::RegisterSighted(USightedComponent* NewSightedComponent)
{
	if (!NewSightedComponent)
	{
		return;
	}
	
	if (IsSightedRegistered(NewSightedComponent))
	{
		return;
	}

	SightedComponents.Add(NewSightedComponent);
	for (const TWeakObjectPtr<USighterComponent>& SighterComponentPtr : SighterComponents.Array())
	{
		USighterComponent* SighterComponent = SighterComponentPtr.Get();
		if (USightSystemFunctionLibrary::ShouldSight(SighterComponent, NewSightedComponent))
		{
			SightQueries.Add(FSightQueryContext(SighterComponent, NewSightedComponent));
		}
	}
}

void USightQueryManager::UnregisterSighted(USightedComponent* NewSightedComponent)
{
	if (!IsSightedRegistered(NewSightedComponent))
	{
		return;
	}
	
	for (int Index = SightQueries.Num() - 1; Index > -1; Index--)
	{
		if (SightQueries[Index].Sighted.HasSameIndexAndSerialNumber(NewSightedComponent))
		{
			AsyncTask(ENamedThreads::GameThread, [NewSightedComponent, Q = SightQueries[Index]]()
			{
				USighterComponent* SighterComponent = Q.Sighter.Get();
				SighterComponent->LoseTarget(NewSightedComponent, Q.bIsPerceived);
				SighterComponent->ForgetTarget(NewSightedComponent);
			});
			SightQueries.RemoveAt(Index, 1, EAllowShrinking::No);
		}
	}

	SightedComponents.Remove(NewSightedComponent);
}

bool USightQueryManager::IsSighterRegistered(const USighterComponent* SighterComponent) const
{
	return SighterComponents.Contains(SighterComponent);
}

void USightQueryManager::RegisterSighter(USighterComponent* NewSighterComponent)
{
	if (!NewSighterComponent)
	{
		return;
	}
	
	if (IsSighterRegistered(NewSighterComponent))
	{
		return;
	}

	SighterComponents.Add(NewSighterComponent);
	for (const TWeakObjectPtr<USightedComponent>& SightedComponentPtr : SightedComponents.Array())
	{
		USightedComponent* SightedComponent = SightedComponentPtr.Get();
		if (USightSystemFunctionLibrary::ShouldSight(NewSighterComponent, SightedComponent))
		{
			SightQueries.Add(FSightQueryContext(NewSighterComponent, SightedComponent));
		}
	}
}

void USightQueryManager::UnregisterSighter(USighterComponent* NewSighterComponent)
{
	if (!IsSighterRegistered(NewSighterComponent))
	{
		return;
	}
	
	for (int Index = SightQueries.Num() - 1; Index > -1; Index--)
	{
		if (SightQueries[Index].Sighter.HasSameIndexAndSerialNumber(NewSighterComponent))
		{
			AsyncTask(ENamedThreads::GameThread, [NewSighterComponent, Q = SightQueries[Index]]()
			{
				USightedComponent* SightedComponent = Q.Sighted.Get();
				NewSighterComponent->LoseTarget(SightedComponent, Q.bIsPerceived);
				NewSighterComponent->ForgetTarget(SightedComponent);
			});
			SightQueries.RemoveAt(Index, 1, EAllowShrinking::No);
		}
	}

	SighterComponents.Remove(NewSighterComponent);
}

float USightQueryManager::GetGainValueForTarget(USighterComponent* SighterComponent, USightedComponent* SightedComponent) const
{
	const FSightQueryContext* Query = SightQueries.FindByPredicate([SighterComponent, SightedComponent](const FSightQueryContext& Query)
	{
		return Query.Sighter.HasSameIndexAndSerialNumber(SighterComponent) && Query.Sighted.HasSameIndexAndSerialNumber(SightedComponent);
	});
	return Query ? Query->GainValue : INDEX_NONE;
}

const USightedComponent* USightQueryManager::GetHighestGainValue(USighterComponent* SighterComponent, float& OutGain) const
{
	const USightedComponent* SightedComponent = nullptr;
	for (const FSightQueryContext& Query : SightQueries)
	{
		if (Query.Sighter.HasSameIndexAndSerialNumber(SighterComponent) &&
			Query.GainValue > OutGain)
		{
			OutGain = Query.GainValue;
			SightedComponent = Query.Sighted.Get();
		}
	}
	return SightedComponent;
}

FVector USightQueryManager::GetLastKnownLocationFor(USighterComponent* SighterComponent, USightedComponent* SightedComponent) const
{
	const FSightQueryContext* Query = SightQueries.FindByPredicate([SighterComponent, SightedComponent](const FSightQueryContext& Query)
	{
		return Query.Sighter.HasSameIndexAndSerialNumber(SighterComponent) && Query.Sighted.HasSameIndexAndSerialNumber(SightedComponent);
	});
	return Query ? Query->LastSeenLocation : FVector::ZeroVector;
}

void USightQueryManager::AddGainToTarget(USighterComponent* SighterComponent, USightedComponent* SightedComponent, const float Value)
{
	FSightQueryContext* Query = SightQueries.FindByPredicate([SighterComponent, SightedComponent](const FSightQueryContext& Query)
	{
		return Query.Sighter.HasSameIndexAndSerialNumber(SighterComponent) && Query.Sighted.HasSameIndexAndSerialNumber(SightedComponent);
	});

	if (!Query)
	{
		return;
	}

	if (Query->bIsPerceived)
	{
		return;
	}

	if (Query->GainValue <= 0.0f || Query->bPreviousCheckSucceeded)
	{
		AsyncTask(ENamedThreads::GameThread, [SighterComponent, SightedComponent]()
		{
			SighterComponent->SpotTarget(SightedComponent);
		});
	}

	Query->GainValue += Value;
	Query->LastSeenLocation = SightedComponent->GetComponentLocation();

	if (Query->GainValue >= Query->Sighter->SightDataAsset->PerceptionGainTime)
	{
		Query->TimeSinceLostSight = 0.0f;
		Query->bIsPerceived = true;
		Query->bPreviousCheckSucceeded = true;

		AsyncTask(ENamedThreads::GameThread, [SighterComponent, SightedComponent]()
		{
			SighterComponent->PerceiveTarget(SightedComponent, false);
		});
	}
}

void USightQueryManager::ForcePerceiveTarget(USighterComponent* SighterComponent, USightedComponent* SightedComponent)
{
	FSightQueryContext* Query = SightQueries.FindByPredicate([SighterComponent, SightedComponent](const FSightQueryContext& Query)
	{
		return Query.Sighter.HasSameIndexAndSerialNumber(SighterComponent) && Query.Sighted.HasSameIndexAndSerialNumber(SightedComponent);
	});
	
	if (!Query)
	{
		return;
	}

	if (Query->GainValue <= 0.0f || !Query->bPreviousCheckSucceeded)
	{
		AsyncTask(ENamedThreads::GameThread, [SighterComponent, SightedComponent]()
		{
			SighterComponent->SpotTarget(SightedComponent);
		});
	}

	if (!Query->bIsPerceived)
	{
		AsyncTask(ENamedThreads::GameThread, [SighterComponent, SightedComponent]()
		{
			SighterComponent->PerceiveTarget(SightedComponent, false);
		});
	}
			
	Query->TimeSinceLostSight = 0.0f;
	Query->bIsPerceived = true;
	Query->bPreviousCheckSucceeded = true;
	Query->LastSeenLocation = SightedComponent->GetComponentLocation();
	Query->GainValue = Query->Sighter->SightDataAsset->PerceptionGainTime + SMALL_NUMBER;
}

void USightQueryManager::ForceForgetTarget(USighterComponent* SighterComponent, USightedComponent* SightedComponent)
{
	FSightQueryContext* Query = SightQueries.FindByPredicate([SighterComponent, SightedComponent](const FSightQueryContext& Query)
	{
		return Query.Sighter.HasSameIndexAndSerialNumber(SighterComponent) && Query.Sighted.HasSameIndexAndSerialNumber(SightedComponent);
	});
	
	if (!Query)
	{
		return;
	}

	if (Query->bPreviousCheckSucceeded)
	{
		AsyncTask(ENamedThreads::GameThread, [SighterComponent, SightedComponent, Query]()
		{
			SighterComponent->LoseTarget(SightedComponent, Query->bIsPerceived);
		});
	}

	if (Query->bIsPerceived)
	{
		AsyncTask(ENamedThreads::GameThread, [SighterComponent, SightedComponent, Query]()
		{
			SighterComponent->ForgetTarget(SightedComponent);
		});
	}

	Query->GainValue = 0.0f;
	Query->TimeSinceLostSight = 0.0f;
	Query->bIsPerceived = false;
	Query->bPreviousCheckSucceeded = false;
}

void USightQueryManager::ForceForgetAllTargets(USighterComponent* SighterComponent)
{
	for (FSightQueryContext& Query : SightQueries)
	{
		if (Query.Sighter.HasSameIndexAndSerialNumber(SighterComponent))
		{
			USightedComponent* SightedComponent = Query.Sighted.Get();
			if (Query.bPreviousCheckSucceeded)
			{
				AsyncTask(ENamedThreads::GameThread, [SighterComponent, SightedComponent, Query]()
				{
					SighterComponent->LoseTarget(SightedComponent, Query.bIsPerceived);
				});
			}

			if (Query.bIsPerceived)
			{
				AsyncTask(ENamedThreads::GameThread, [SighterComponent, SightedComponent]()
				{
					SighterComponent->ForgetTarget(SightedComponent);
				});
			}

			Query.GainValue = 0.0f;
			Query.TimeSinceLostSight = 0.0f;
			Query.bIsPerceived = false;
			Query.bPreviousCheckSucceeded = false;
		}
	}
}

bool USightQueryManager::IsSpotted(USightedComponent* SightedComponent, TArray<USighterComponent*>& OutSighters) const
{
	GetSightersByCondition(SightedComponent, OutSighters, [](const FSightQueryContext& Query)
	{
		return Query.bPreviousCheckSucceeded && !Query.bIsPerceived;
	});
	return OutSighters.Num() > 0;
}

bool USightQueryManager::IsPerceived(USightedComponent* SightedComponent, TArray<USighterComponent*>& OutSighters) const
{
	GetSightersByCondition(SightedComponent, OutSighters, [](const FSightQueryContext& Query)
	{
		return !!Query.bIsPerceived;
	});

	return OutSighters.Num() > 0;
}

bool USightQueryManager::IsLost(USightedComponent* SightedComponent, TArray<USighterComponent*>& OutSighters) const
{
	GetSightersByCondition(SightedComponent, OutSighters, [](const FSightQueryContext& Query)
	{
		return Query.TimeSinceLostSight > 0.0f
			&& Query.TimeSinceLostSight < Query.Sighter->SightDataAsset->LoseSightCooldownTime
			&& !Query.bPreviousCheckSucceeded;
	});

	return OutSighters.Num() > 0;
}

void USightQueryManager::GetSightersByCondition(USightedComponent* SightedComponent, TArray<USighterComponent*>& OutSighters, const TFunction<bool(const FSightQueryContext&)>& Condition) const
{
	for (const FSightQueryContext& Query : SightQueries)
	{
		if (!Query.Sighted.HasSameIndexAndSerialNumber(SightedComponent))
		{
			continue;
		}
		
		if (Condition(Query))
		{
			OutSighters.Add(Query.Sighter.Get());
		}
	}
}

void USightQueryManager::HandleSuccessfulCheck(FSightQueryContext& Q, const float Dt, const FVector& CurrentPos)
{
	if (!Q.Sighter.IsValid() || !Q.Sighted.IsValid())
	{
		return;
	}
	
	const USightSystemDataAsset* Data = Q.Sighter->SightDataAsset;
    const auto& SighterM = Q.Sighter->SightSystemMultipliers;
    const auto& SightedM = Q.Sighted->SightSystemMultipliers;
	
	const bool bFirstFrame = !Q.bPreviousCheckSucceeded;
	Q.bPreviousCheckSucceeded = true;

	// Enter perceived / spotted state
	if (bFirstFrame)
	{
		if (Q.bIsPerceived)
		{
			AsyncTask(ENamedThreads::GameThread, [Q]()
			{
				Q.Sighter->PerceiveTarget(Q.Sighted.Get(), true);
			});
		}
		else
		{
			AsyncTask(ENamedThreads::GameThread, [Q]()
			{
				Q.Sighter->SpotTarget(Q.Sighted.Get());
			});
		}
	}

	// Always update the last‑seen location
	Q.LastSeenLocation = CurrentPos;

	// Either keep gaining or keep timer at zero
	if (!Q.bIsPerceived)
	{
		Q.GainValue += Dt * Q.GainRate * SighterM.GainRate * SightedM.GainRate;

		if (Q.GainValue >= Data->PerceptionGainTime)
		{
			Q.bIsPerceived = true;
			AsyncTask(ENamedThreads::GameThread, [Q]()
			{
				Q.Sighter->PerceiveTarget(Q.Sighted.Get(), false);
			});
		}
	}
	else
	{
		Q.TimeSinceLostSight = 0.f;
	}
}

void USightQueryManager::HandlePerceivedLoss(FSightQueryContext& Q, const float Dt, const FVector& CurrentPos)
{
	if (!Q.Sighter.IsValid() || !Q.Sighted.IsValid())
	{
		return;
	}
	
	const USightSystemDataAsset* Data = Q.Sighter->SightDataAsset;
	const auto& SighterM = Q.Sighter->SightSystemMultipliers;
	const auto& SightedM = Q.Sighted->SightSystemMultipliers;
	
	const float DistSq = FVector::DistSquared(CurrentPos, Q.LastSeenLocation);
	const float ToleranceDistSq = FMath::Square(Data->LostSightToleranceDist);

	if (DistSq < ToleranceDistSq)
	{
		return; // still inside tolerance -> nothing to do
	}

	// First frame outside tolerance -> fire LoseTarget(perceived)
	if (Q.bPreviousCheckSucceeded)
	{
		Q.bPreviousCheckSucceeded = false;
		Q.Sighter->LoseTarget(Q.Sighted.Get(), /*bWasPerceived*/true);
	}

	// Accumulate forget time
	Q.TimeSinceLostSight += Dt * SighterM.ForgetTime * SightedM.ForgetTime;

	if (Q.TimeSinceLostSight >= Data->LoseSightCooldownTime)
	{
		Q.bIsPerceived = false;
		Q.GainValue = 0.f;
		Q.TimeSinceLostSight = 0.f;
		Q.Sighter->ForgetTarget(Q.Sighted.Get());
	}
}

void USightQueryManager::HandleSpottedLoss(FSightQueryContext& Q, const float Dt)
{
	if (!Q.Sighter.IsValid() || !Q.Sighted.IsValid())
	{
		return;
	}
	
	const USightSystemDataAsset*  Data = Q.Sighter->SightDataAsset;
	const auto& SighterM = Q.Sighter->SightSystemMultipliers;
	const auto& SightedM = Q.Sighted->SightSystemMultipliers;
	
	// First frame of failure -> start grace timer
	if (Q.bPreviousCheckSucceeded)
	{
		Q.bPreviousCheckSucceeded = false;
		Q.WaitTime = 0.f;
		AsyncTask(ENamedThreads::GameThread, [Q]()
		{
			Q.Sighter->StartTargetLostWait(Q.Sighted.Get());
		});
	}

	// Grace‑time counting
	if (Q.WaitTime <= Data->SpottedGraceTime)
	{
		Q.WaitTime += Dt * SighterM.WaitTime * SightedM.WaitTime;

		if (Q.WaitTime > Data->SpottedGraceTime)
		{
			Q.Sighter->LoseTarget(Q.Sighted.Get(), false);
		}
	}
	else  // grace elapsed -> decay accumulated gain
	{
		const float Decay = Dt * Data->GainDecayRate * SightedM.GainDecayRate * SighterM.GainDecayRate;
		Q.GainValue = FMath::Max(0.f, Q.GainValue - Decay);
	}
}

void USightQueryManager::ProcessQueryState(FSightQueryContext& Q, float Dt)
{
	if (!Q.Sighter.IsValid() || !Q.Sighted.IsValid())
	{
		return;
	}

	const FVector CurrentPos = Q.Sighted->GetComponentLocation();
	if (Q.bCurrenCheckSucceeded)
	{
		HandleSuccessfulCheck(Q, Dt, CurrentPos);
		return;
	}

	if (Q.bIsPerceived)
	{
		HandlePerceivedLoss(Q, Dt, CurrentPos);
		return;
	}

	HandleSpottedLoss(Q, Dt);
}

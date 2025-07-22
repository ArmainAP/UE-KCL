// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/TeamPerceptionTrackerComponent.h"

#include "AIController.h"
#include "Logging.h"

// Sets default values for this component's properties
UTeamPerceptionTrackerComponent::UTeamPerceptionTrackerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UTeamPerceptionTrackerComponent::IsEmpty(const ETeamAttitude::Type TeamAttitude) const
{
	return GetActorsSet(TeamAttitude).IsEmpty();
}

void UTeamPerceptionTrackerComponent::OnActorEndPlay(AActor* Actor, EEndPlayReason::Type EndPlayReason)
{
	HandleForgotten(Actor);
}

FPerceivedActorSet& UTeamPerceptionTrackerComponent::SelectSet(
	const UTeamPerceptionTrackerComponent* TeamPerceptionTrackerComponent, const ETeamAttitude::Type TeamAttitude)
{
	switch (TeamAttitude)
	{
	case ETeamAttitude::Friendly: return TeamPerceptionTrackerComponent->FriendlyActors;
	case ETeamAttitude::Neutral: return TeamPerceptionTrackerComponent->NeutralActors;
	case ETeamAttitude::Hostile: return TeamPerceptionTrackerComponent->HostileActors;
	default: return TeamPerceptionTrackerComponent->NeutralActors;
	}
}

const FPerceivedActorSet& UTeamPerceptionTrackerComponent::GetActorsSet(const ETeamAttitude::Type TeamAttitude) const
{
	return SelectSet(this, TeamAttitude);
}

void UTeamPerceptionTrackerComponent::HandleForgotten(AActor* Actor)
{
	int ElementsRemoved = 0;
	ElementsRemoved += FriendlyActors.Remove(Actor);
	ElementsRemoved += NeutralActors.Remove(Actor);
	ElementsRemoved += HostileActors.Remove(Actor);

	if (!ElementsRemoved)
	{
		return;
	}

	const IGenericTeamAgentInterface* GenericTeamAgent = Cast<IGenericTeamAgentInterface>(GetOwner());
	if (!GenericTeamAgent)
	{
		UE_LOG(LogKCL, Error, TEXT("%s >> Owner does not implement IGenericTeamAgentInterface"), StringCast<TCHAR>(__FUNCTION__).Get());
		return;
	}
	
	const ETeamAttitude::Type Attitude = GenericTeamAgent->GetTeamAttitudeTowards(*Actor);
	OnForgotten.Broadcast(Attitude, Actor);
}

void UTeamPerceptionTrackerComponent::GetActorsContainer(const ETeamAttitude::Type TeamAttitude, TArray<AActor*>& OutActors)
{
	OutActors.Reset();
	for (auto It = SelectSet(this, TeamAttitude).CreateIterator(); It; ++It)
	{
		if (It->IsValid())
		{
			OutActors.AddUnique(It->Get());
		}
		else
		{
			It.RemoveCurrent();
		}
	}
}

void UTeamPerceptionTrackerComponent::HandlePerceived(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return;
	}

	const IGenericTeamAgentInterface* GenericTeamAgent = Cast<IGenericTeamAgentInterface>(GetOwner());
	if (!GenericTeamAgent)
	{
		UE_LOG(LogKCL, Error, TEXT("%s >> Owner does not implement IGenericTeamAgentInterface"), StringCast<TCHAR>(__FUNCTION__).Get());
		return;
	}

	const ETeamAttitude::Type Attitude = GenericTeamAgent->GetTeamAttitudeTowards(*Actor);	
	FPerceivedActorSet& Source = SelectSet(this, Attitude);
	if (Source.Contains(Actor))
	{
		return;
	}

	Actor->OnEndPlay.AddUniqueDynamic(this, &UTeamPerceptionTrackerComponent::OnActorEndPlay);
	HandleForgotten(Actor);	
	Source.Add(Actor);
	OnPerceived.Broadcast(Attitude, Actor);
}
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
	const TSet<TObjectPtr<AActor>>& Source = GetActorsContainer(TeamAttitude);
	return Source.IsEmpty();
}

void UTeamPerceptionTrackerComponent::HandleForgotten(AActor* Actor)
{
	FriendlyActors.Remove(Actor);
	NeutralActors.Remove(Actor);
	HostileActors.Remove(Actor);

	const IGenericTeamAgentInterface* GenericTeamAgent = Cast<IGenericTeamAgentInterface>(GetOwner());
	if (!GenericTeamAgent)
	{
		UE_LOG(LogKCL, Error, TEXT("%s >> Owner does not implement IGenericTeamAgentInterface"), StringCast<TCHAR>(__FUNCTION__).Get());
		return;
	}
	
	const ETeamAttitude::Type Attitude = GenericTeamAgent->GetTeamAttitudeTowards(*Actor);
	OnForgotten.Broadcast(Attitude, Actor);
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
	TSet<TObjectPtr<AActor>>& Source = GetMutableActorsContainer(Attitude);
	if (Source.Contains(Actor))
	{
		return;
	}

	HandleForgotten(Actor);	
	Source.Add(Actor);
	OnPerceived.Broadcast(Attitude, Actor);
}

TSet<TObjectPtr<AActor>>& UTeamPerceptionTrackerComponent::GetMutableActorsContainer(const ETeamAttitude::Type TeamAttitude)
{
	switch (TeamAttitude)
	{
	case ETeamAttitude::Friendly: return FriendlyActors;
	case ETeamAttitude::Neutral: return NeutralActors;
	case ETeamAttitude::Hostile: return HostileActors;
	default: return NeutralActors;
	}
}

const TSet<TObjectPtr<AActor>>& UTeamPerceptionTrackerComponent::GetActorsContainer(const ETeamAttitude::Type TeamAttitude) const
{
	switch (TeamAttitude)
	{
	case ETeamAttitude::Friendly: return FriendlyActors;
	case ETeamAttitude::Neutral: return NeutralActors;
	case ETeamAttitude::Hostile: return HostileActors;
	default: return NeutralActors;
	}
}

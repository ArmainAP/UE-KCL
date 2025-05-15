// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/TeamPerceptionTrackerComponent.h"

#include "AIController.h"
#include "Logging.h"

// Sets default values for this component's properties
UTeamPerceptionTrackerComponent::UTeamPerceptionTrackerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTeamPerceptionTrackerComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedOwnerTeamAgentInterface = Cast<IGenericTeamAgentInterface>(GetOwner());
	if (!CachedOwnerTeamAgentInterface.IsValid())
	{
		UE_LOG(LogKCL, Error, TEXT("%s >> Owner does not implement IGenericTeamAgentInterface"), StringCast<TCHAR>(__FUNCTION__).Get());
	}
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

	if (!CachedOwnerTeamAgentInterface.IsValid())
	{
		return;
	}
	
	const ETeamAttitude::Type Attitude = CachedOwnerTeamAgentInterface->GetTeamAttitudeTowards(*Actor);
	OnForgotten.Broadcast(Attitude, Actor);
}

void UTeamPerceptionTrackerComponent::HandlePerceived(AActor* Actor)
{
	if (!IsValid(Actor) || !CachedOwnerTeamAgentInterface.IsValid())
	{
		return;
	}

	const ETeamAttitude::Type Attitude = CachedOwnerTeamAgentInterface->GetTeamAttitudeTowards(*Actor);
	TSet<TObjectPtr<AActor>>& Source = GetMutableActorsContainer(Attitude);
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

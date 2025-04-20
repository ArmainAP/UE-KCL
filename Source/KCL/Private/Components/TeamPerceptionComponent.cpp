// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/TeamPerceptionComponent.h"

#include "AIController.h"
#include "Logging.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Perception/AIPerceptionComponent.h"

// Sets default values for this component's properties
UTeamPerceptionComponent::UTeamPerceptionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = false;
}

void UTeamPerceptionComponent::Activate(bool bReset)
{
	CachedOwner = GetOwner();
	if (!IsValid(CachedOwner))
	{
		UE_LOG(LogKCL, Warning, TEXT("%s called with NULL Owner"), TEXT(__FUNCTION__));
		return;
	}

	AAIController* AIC = UAIBlueprintHelperLibrary::GetAIController(CachedOwner);
	if (!IsValid(AIC))
	{
		UE_LOG(LogKCL, Warning, TEXT("%s called with NULL AI Controller"), TEXT(__FUNCTION__));
		return;
	}

	PerceptionComponent = AIC->GetComponentByClass<UAIPerceptionComponent>();
	if (!IsValid(PerceptionComponent))
	{
		UE_LOG(LogKCL, Warning, TEXT("%s did not find AI Perception Component"), TEXT(__FUNCTION__));
		return;
	}

	PerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &UTeamPerceptionComponent::OnTargetPerceptionUpdated);
	PerceptionComponent->OnTargetPerceptionForgotten.AddUniqueDynamic(this, &UTeamPerceptionComponent::OnTargetPerceptionForgotten);
	PerceptionComponent->OnTargetPerceptionInfoUpdated.AddUniqueDynamic(this, &UTeamPerceptionComponent::OnTargetPerceptionInfoUpdated);
	
	Super::Activate(bReset);
}

void UTeamPerceptionComponent::Deactivate()
{
	PerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(this, &UTeamPerceptionComponent::OnTargetPerceptionUpdated);
	PerceptionComponent->OnTargetPerceptionForgotten.RemoveDynamic(this, &UTeamPerceptionComponent::OnTargetPerceptionForgotten);
	PerceptionComponent->OnTargetPerceptionInfoUpdated.RemoveDynamic(this, &UTeamPerceptionComponent::OnTargetPerceptionInfoUpdated);
	
	Super::Deactivate();
}

void UTeamPerceptionComponent::SetTeamID(const uint8 InTeamID)
{
	TeamID = InTeamID;
}

uint8 UTeamPerceptionComponent::GetTeamID() const
{
	return TeamID;
}

void UTeamPerceptionComponent::GetPerceivedActors(const ETeamAttitude::Type TeamAttitude, TSet<AActor*>& OutPerceivedActors) const
{
	const TSet<TWeakObjectPtr<AActor>>& Source = GetActorsContainer(TeamAttitude);
	OutPerceivedActors.Reset();
	OutPerceivedActors.Reserve(Source.Num());
	Algo::TransformIf(Source, OutPerceivedActors,
		/* Predicate  */ [](const TWeakObjectPtr<AActor>& W){ return W.IsValid(); },
		/* Transform  */ [](const TWeakObjectPtr<AActor>& W){ return W.Get(); });
}

void UTeamPerceptionComponent::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		HandlePerceived(Actor);
	}
	else
	{
		OnTargetPerceptionForgotten(Actor);
	}
}

void UTeamPerceptionComponent::OnTargetPerceptionForgotten(AActor* Actor)
{
	FriendlyActors.Remove(Actor);
	NeutralActors.Remove(Actor);
	HostileActors.Remove(Actor);
}

void UTeamPerceptionComponent::OnTargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	if (!UpdateInfo.Stimulus.WasSuccessfullySensed())
	{
		OnTargetPerceptionForgotten(UpdateInfo.Target.Get());
	}
}

void UTeamPerceptionComponent::HandlePerceived(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return;
	}

	const IGenericTeamAgentInterface* GenericTeamAgentInterface = Cast<IGenericTeamAgentInterface>(CachedOwner);
	if (!GenericTeamAgentInterface)
	{
		return;
	}
	
	TSet<TWeakObjectPtr<AActor>>& Source = GetMutableActorsContainer(GenericTeamAgentInterface->GetTeamAttitudeTowards(*Actor));
	OnTargetPerceptionForgotten(Actor);
	Source.Add(Actor);
}

TSet<TWeakObjectPtr<AActor>>& UTeamPerceptionComponent::GetMutableActorsContainer(const ETeamAttitude::Type TeamAttitude)
{
	switch (TeamAttitude)
	{
	case ETeamAttitude::Friendly: return FriendlyActors;
	case ETeamAttitude::Neutral: return NeutralActors;
	case ETeamAttitude::Hostile: return HostileActors;
	default: return NeutralActors;
	}
}

const TSet<TWeakObjectPtr<AActor>>& UTeamPerceptionComponent::GetActorsContainer(const ETeamAttitude::Type TeamAttitude) const
{
	switch (TeamAttitude)
	{
	case ETeamAttitude::Friendly: return FriendlyActors;
	case ETeamAttitude::Neutral: return NeutralActors;
	case ETeamAttitude::Hostile: return HostileActors;
	default: return NeutralActors;
	}
}

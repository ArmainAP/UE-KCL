// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "GameplayAbilities/GameplayAbility_InputInteract.h"
#include "Components/InputInteractorComponent.h"

UGameplayAbility_InputInteract::UGameplayAbility_InputInteract(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGameplayAbility_InputInteract::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	InputInteractorComponent = ActorInfo->AvatarActor.Get()->GetComponentByClass<UInputInteractorComponent>();
	if (InputInteractorComponent.IsValid())
	{
		InputInteractorComponent->InteractionInput(InputTag, true);
	}
}

void UGameplayAbility_InputInteract::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	if (InputInteractorComponent.IsValid())
	{
		InputInteractorComponent->InteractionInput(InputTag, false);
	}
	
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGameplayAbility_InputInteract::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (InputInteractorComponent.IsValid())
	{
		InputInteractorComponent->InteractionInput(InputTag, false);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGameplayAbility_InputInteract::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

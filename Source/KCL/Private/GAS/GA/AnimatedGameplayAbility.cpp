// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "GAS/GA/AnimatedGameplayAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void UAnimatedGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                               const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (bPlayOnActivation)
	{
		PlayMontage();
	}
}

void UAnimatedGameplayAbility::PlayMontage()
{
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, GetFName(), AnimMontage, Rate, StartSection, bStopWhenAbilityEnds, AnimRootMotionTranslationScale, StartTimeSeconds, bAllowInterruptAfterBlendOut);
	MontageTask->OnCompleted.AddDynamic(this, &UAnimatedGameplayAbility::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UAnimatedGameplayAbility::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UAnimatedGameplayAbility::OnMontageCancelled);
	MontageTask->OnBlendedIn.AddDynamic(this, &UAnimatedGameplayAbility::OnBlendedIn);
	MontageTask->OnBlendOut.AddDynamic(this, &UAnimatedGameplayAbility::OnBlendOut);
	MontageTask->ReadyForActivation();
}

void UAnimatedGameplayAbility::OnMontageCompleted_Implementation()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAnimatedGameplayAbility::OnMontageInterrupted_Implementation()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}

void UAnimatedGameplayAbility::OnMontageCancelled_Implementation()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}

void UAnimatedGameplayAbility::OnBlendedIn_Implementation()
{
}

void UAnimatedGameplayAbility::OnBlendOut_Implementation()
{
}

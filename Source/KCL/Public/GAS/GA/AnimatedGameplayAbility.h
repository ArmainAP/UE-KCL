// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbility_Montage.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AnimatedGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class KCL_API UAnimatedGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION(BlueprintCallable)
	void PlayMontage();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnMontageCompleted();

	UFUNCTION(BlueprintNativeEvent)
	void OnMontageInterrupted();

	UFUNCTION(BlueprintNativeEvent)
	void OnMontageCancelled();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnBlendedIn();

	UFUNCTION(BlueprintNativeEvent)
	void OnBlendOut();

	UPROPERTY(EditAnywhere, Category=Ability)
	bool bPlayOnActivation;
	
	UPROPERTY(EditAnywhere, Category=Montage)
	TObjectPtr<UAnimMontage> AnimMontage;

	UPROPERTY(EditAnywhere, Category=Montage)
	float Rate = 1.f;

	UPROPERTY(EditAnywhere, Category=Montage)
	FName StartSection = NAME_None;

	UPROPERTY(EditAnywhere, Category=Montage)
	bool bStopWhenAbilityEnds = true;

	UPROPERTY(EditAnywhere, Category=Montage)
	float AnimRootMotionTranslationScale = 1.f;

	UPROPERTY(EditAnywhere, Category=Montage)
	float StartTimeSeconds = 0.f;

	UPROPERTY(EditAnywhere, Category=Montage)
	bool bAllowInterruptAfterBlendOut = false;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
};

// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "TeamPerceptionTrackerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTeamPerceptionTrackerEvent, const ETeamAttitude::Type, TeamAttitude, AActor*, Actor);

typedef TSet<TWeakObjectPtr<AActor>, TWeakObjectPtrSetKeyFuncs<TWeakObjectPtr<AActor>>> FPerceivedActorSet;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KCL_API UTeamPerceptionTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTeamPerceptionTrackerComponent();
	
	UFUNCTION(BlueprintCallable)
	void HandlePerceived(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void HandleForgotten(AActor* Actor);

	const FPerceivedActorSet& GetActorsSet(const ETeamAttitude::Type TeamAttitude) const;

	UFUNCTION(BlueprintCallable)
	void GetActorsContainer(const ETeamAttitude::Type TeamAttitude, TArray<AActor*>& OutActors);

	UFUNCTION(BlueprintPure)
	bool IsEmpty(ETeamAttitude::Type TeamAttitude) const;

	UPROPERTY(BlueprintAssignable)
	FTeamPerceptionTrackerEvent OnPerceived;

	UPROPERTY(BlueprintAssignable)
	FTeamPerceptionTrackerEvent OnForgotten;

protected:
	UFUNCTION()
	void OnActorEndPlay(AActor* Actor, EEndPlayReason::Type EndPlayReason);

	static FPerceivedActorSet& SelectSet(const UTeamPerceptionTrackerComponent* TeamPerceptionTrackerComponent, const ETeamAttitude::Type TeamAttitude);
	mutable FPerceivedActorSet FriendlyActors;
	mutable FPerceivedActorSet NeutralActors;
	mutable FPerceivedActorSet HostileActors;
};
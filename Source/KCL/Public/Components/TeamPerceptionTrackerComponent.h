// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "TeamPerceptionTrackerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTeamPerceptionTrackerEvent, const ETeamAttitude::Type, TeamAttitude, AActor*, Actor);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KCL_API UTeamPerceptionTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTeamPerceptionTrackerComponent();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void HandlePerceived(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void HandleForgotten(AActor* Actor);
	
	const TSet<TObjectPtr<AActor>>& GetActorsContainer(const ETeamAttitude::Type TeamAttitude) const;
	TSet<TObjectPtr<AActor>>& GetMutableActorsContainer(const ETeamAttitude::Type TeamAttitude);

	UFUNCTION(BlueprintPure)
	bool IsEmpty(ETeamAttitude::Type TeamAttitude) const;

	UPROPERTY(BlueprintAssignable)
	FTeamPerceptionTrackerEvent OnPerceived;

	UPROPERTY(BlueprintAssignable)
	FTeamPerceptionTrackerEvent OnForgotten;
	
protected:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSet<TObjectPtr<AActor>> FriendlyActors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSet<TObjectPtr<AActor>> NeutralActors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSet<TObjectPtr<AActor>> HostileActors;

	TWeakInterfacePtr<IGenericTeamAgentInterface> CachedOwnerTeamAgentInterface;
};
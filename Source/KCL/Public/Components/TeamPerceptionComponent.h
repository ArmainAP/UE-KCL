// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "TeamPerceptionComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KCL_API UTeamPerceptionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTeamPerceptionComponent();
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;

	UFUNCTION(BlueprintCallable)
	void SetTeamID(const uint8 InTeamID);
	
	UFUNCTION(BlueprintCallable)
	uint8 GetTeamID() const;

	UFUNCTION(BlueprintPure)
	void GetPerceivedActorsSet(ETeamAttitude::Type TeamAttitude, TSet<AActor*>& OutPerceivedActors) const;
	
	UFUNCTION(BlueprintPure)
	void GetPerceivedActorsArray(ETeamAttitude::Type TeamAttitude, TArray<AActor*>& OutPerceivedActors) const;
	
	template <class OutT>
	void GetPerceivedActors(ETeamAttitude::Type TeamAttitude, OutT&& OutPerceivedActors) const;

	const TSet<TWeakObjectPtr<AActor>>& GetActorsContainer(const ETeamAttitude::Type TeamAttitude) const;

	UFUNCTION(BlueprintPure)
	bool IsEmpty(ETeamAttitude::Type TeamAttitude) const;

protected:
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	UFUNCTION()
    void OnTargetPerceptionForgotten(AActor* Actor);

	UFUNCTION()
	void OnTargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo);

	void HandlePerceived(AActor* Actor);

	TSet<TWeakObjectPtr<AActor>>& GetMutableActorsContainer(const ETeamAttitude::Type TeamAttitude);
	
	UPROPERTY(EditAnywhere)
	uint8 TeamID = 0;

	UPROPERTY(VisibleAnywhere)
	TSet<TWeakObjectPtr<AActor>> FriendlyActors;

	UPROPERTY(VisibleAnywhere)
	TSet<TWeakObjectPtr<AActor>> NeutralActors;

	UPROPERTY(VisibleAnywhere)
	TSet<TWeakObjectPtr<AActor>> HostileActors;

	UPROPERTY()
	TObjectPtr<AActor> CachedOwner;
	
	UPROPERTY()
	TObjectPtr<UAIPerceptionComponent> PerceptionComponent;
};
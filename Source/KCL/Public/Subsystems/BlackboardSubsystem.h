// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Objects/BlackboardContext.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BlackboardSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class KCL_API UBlackboardSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION(BlueprintPure)
	UBlackboardContext* GetGlobalContext() const;
	
	UFUNCTION(BlueprintCallable)
	UBlackboardContext* RegisterActorContext(AActor* Owner);

	UFUNCTION(BlueprintPure)
	UBlackboardContext* GetActorContext(AActor* Owner) const;

private:
	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, TObjectPtr<UBlackboardContext>> ActorContexts;

	UPROPERTY()
	TObjectPtr<UBlackboardContext> GlobalContext;
	
	UFUNCTION()
	void HandleActorDestroyed(AActor* DeadActor);
};
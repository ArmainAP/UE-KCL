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
	UFUNCTION(BlueprintPure)
	UBlackboardContext* GetGlobalContext();
	
	UFUNCTION(BlueprintCallable)
	UBlackboardContext* GetActorContext(AActor* Owner);

	UFUNCTION(BlueprintCallable)
	static UBlackboardContext* Static_GetActorContext(AActor* Owner);

private:
	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, TObjectPtr<UBlackboardContext>> ActorContexts;

	UPROPERTY()
	TObjectPtr<UBlackboardContext> GlobalContext;
	
	UFUNCTION()
	void HandleActorDestroyed(AActor* DeadActor);
};
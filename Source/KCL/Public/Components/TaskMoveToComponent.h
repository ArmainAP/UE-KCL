// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Wrappers/AITaskMoveToWrapper.h"
#include "TaskMoveToComponent.generated.h"

class UTaskMoveToComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTaskMoveToComponentEvent, UTaskMoveToComponent*, TaskMoveToComponent, UAITask_MoveTo_Wrapper*, MoveToContext);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KCL_API UTaskMoveToComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintGetter)
	const FMoveTaskParameters& GetMoveTaskParameters() const { return MoveTaskParameters; }

	UFUNCTION(BlueprintSetter)
	void SetMoveTaskParameters(const FMoveTaskParameters& InMoveTaskParameters) { MoveTaskParameters = InMoveTaskParameters; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StopMovement();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UAITask_MoveTo_Wrapper* MoveToLocation(const FVector& Location, bool bForce = false);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UAITask_MoveTo_Wrapper* MoveToActor(const AActor* Actor, bool bForce = false);

	UFUNCTION(BlueprintPure)
	bool GetDesiredLocation(FVector& OutLocation) const;

	UFUNCTION(BlueprintGetter)
	UAITask_MoveTo_Wrapper* GetMoveToContext() const { return MoveToContext; };

	UPROPERTY(BlueprintAssignable)
	FTaskMoveToComponentEvent OnMoveStarted;

	UPROPERTY(BlueprintAssignable)
	FTaskMoveToComponentEvent OnMoveStopped;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintGetter=GetMoveTaskParameters, BlueprintSetter=SetMoveTaskParameters, Category=MoveTo)
	FMoveTaskParameters MoveTaskParameters;

	UPROPERTY(BlueprintGetter=GetMoveToContext)
	TObjectPtr<UAITask_MoveTo_Wrapper> MoveToContext;
};

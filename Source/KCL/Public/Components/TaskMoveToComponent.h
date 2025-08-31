// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Wrappers/AITaskMoveToWrapper.h"
#include "TaskMoveToComponent.generated.h"

class UTaskMoveToComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMoveToContextEvent, UTaskMoveToComponent*, TaskMoveToComponent, int, ContextID);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KCL_API UTaskMoveToComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTaskMoveToComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintGetter)
	const FMoveTaskParameters& GetMoveTaskParameters() const { return MoveTaskParameters; }

	UFUNCTION(BlueprintSetter)
	void SetMoveTaskParameters(const FMoveTaskParameters& InMoveTaskParameters) { MoveTaskParameters = InMoveTaskParameters; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StopMovement();

	UFUNCTION(BlueprintCallable)
	void InvalidateMoveTask();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int MoveToLocation(const FVector& Location, bool bForce = false);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int MoveToActor(const AActor* Actor, bool bForce = false);

	UFUNCTION(BlueprintPure)
	bool GetDesiredLocation(FVector& OutLocation) const;

	UFUNCTION(BlueprintPure)
	bool IsAtLocation(const FVector& Location) const;

	UFUNCTION(BlueprintGetter)
	UAITask_MoveTo_Wrapper* GetMoveToContext() const { return MoveToContext; };

	UPROPERTY(BlueprintAssignable)
	FMoveToContextEvent OnMoveStarted;

	UPROPERTY(BlueprintAssignable)
	FMoveToContextEvent OnMoveStopped;

	UPROPERTY(BlueprintAssignable)
	FMoveToContextEvent OnContextInvalidated;
	
	UPROPERTY(BlueprintAssignable)
	FMoveToContextEvent OnContextFailed;
	
	UPROPERTY(BlueprintAssignable)
	FMoveToContextEvent OnContextFinished;

protected:
	void SetupTask(const FVector& Location, const AActor* Actor = nullptr);

	UFUNCTION()
	void HandleRequestFailed();

	UFUNCTION()
	void HandleMoveFinished(TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController);
	
	UPROPERTY(EditDefaultsOnly, BlueprintGetter=GetMoveTaskParameters, BlueprintSetter=SetMoveTaskParameters, Category=MoveTo)
	FMoveTaskParameters MoveTaskParameters;

	UPROPERTY(BlueprintGetter=GetMoveToContext)
	TObjectPtr<UAITask_MoveTo_Wrapper> MoveToContext;
};

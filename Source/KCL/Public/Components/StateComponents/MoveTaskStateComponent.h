// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/LeafStateComponent.h"
#include "Components/TaskMoveToComponent.h"
#include "MoveTaskStateComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KCL_API UMoveTaskStateComponent : public ULeafStateComponent
{
	GENERATED_BODY()

protected:
	virtual void StateEnter_Implementation() override;
	virtual void StateExit_Implementation() override;

	UFUNCTION()
	void OnMoveStarted(UTaskMoveToComponent* TaskMoveToComponent, int ContextID);
	
	UFUNCTION()
	void OnContextInvalidated(UTaskMoveToComponent* TaskMoveToComponent, int ContextID);
	
	UFUNCTION()
	void OnContextFailed(UTaskMoveToComponent* TaskMoveToComponent, int ContextID);
	
	UFUNCTION()
	void OnContextFinished(UTaskMoveToComponent* TaskMoveToComponent, int ContextID);
	
	UFUNCTION()
	void OnMoveStopped(UTaskMoveToComponent* TaskMoveToComponent, int ContextID);

	UFUNCTION(BlueprintNativeEvent)
	void HandleMoveStarted(UTaskMoveToComponent* TaskMoveToComponent, int ContextID);
	virtual void HandleMoveStarted_Implementation(UTaskMoveToComponent* TaskMoveToComponent, int ContextID) {}
	
	UFUNCTION(BlueprintNativeEvent)
	void HandleContextFinished(UTaskMoveToComponent* TaskMoveToComponent, int ContextID);
	virtual void HandleContextFinished_Implementation(UTaskMoveToComponent* TaskMoveToComponent, int ContextID) {}
	
	UFUNCTION(BlueprintNativeEvent)
	void HandleContextInvalidated(UTaskMoveToComponent* TaskMoveToComponent, int ContextID);
	
	UFUNCTION(BlueprintNativeEvent)
	void HandleContextFailed(UTaskMoveToComponent* TaskMoveToComponent, int ContextID);
	
	UFUNCTION(BlueprintNativeEvent)
	void HandleMoveStopped(UTaskMoveToComponent* TaskMoveToComponent, int ContextID);

	UFUNCTION(BlueprintNativeEvent)
	void ExecuteMoveTask();
	virtual void ExecuteMoveTask_Implementation() {};

	void BindMoveEvents();
	void UnbindMoveEvents();
	bool CanAutoRestart() const;

	UPROPERTY()
	TWeakObjectPtr<UTaskMoveToComponent> OwnerTaskMoveToComponent;

	UPROPERTY()
	int MoveTaskID = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bBindEventsOnEnter = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUnbindEventsOnExit = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AutoRestart)
	bool bRestartMoveOnInvalidation = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AutoRestart)
	bool bRestartMoveOnFailure = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AutoRestart)
	bool bRestartMoveOnStop = true;

	// INDEX_NONE means no limit. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AutoRestart)
	int AutoRestartLimit = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category=AutoRestart)
	int AutoRestartCounter = INDEX_NONE;
};

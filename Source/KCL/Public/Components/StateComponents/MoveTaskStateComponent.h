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
	void HandleContextInvalidated(UTaskMoveToComponent* TaskMoveToComponent, int ContextID);
	virtual void HandleContextInvalidated_Implementation(UTaskMoveToComponent* TaskMoveToComponent, int ContextID) {}
	
	UFUNCTION(BlueprintNativeEvent)
	void HandleContextFailed(UTaskMoveToComponent* TaskMoveToComponent, int ContextID);
	virtual void HandleContextFailed_Implementation(UTaskMoveToComponent* TaskMoveToComponent, int ContextID) {}
	
	UFUNCTION(BlueprintNativeEvent)
	void HandleContextFinished(UTaskMoveToComponent* TaskMoveToComponent, int ContextID);
	virtual void HandleContextFinished_Implementation(UTaskMoveToComponent* TaskMoveToComponent, int ContextID) {}
	
	UFUNCTION(BlueprintNativeEvent)
	void HandleMoveStopped(UTaskMoveToComponent* TaskMoveToComponent, int ContextID);
	virtual void HandleMoveStopped_Implementation(UTaskMoveToComponent* TaskMoveToComponent, int ContextID) {}

	UPROPERTY()
	TWeakObjectPtr<UTaskMoveToComponent> OwnerTaskMoveToComponent;

	UPROPERTY()
	int MoveTaskID = INDEX_NONE;
};

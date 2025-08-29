// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "EventBusDataAsset.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTaskQueueEvent, UEventBusDataAsset*, InDataAsset, AActor*, InExecutor);

UCLASS(Blueprintable, BlueprintType)
class GLOBALEVENTBUS_API UEventBusDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UEventBusDataAsset();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StartExecution(AActor* InExecutor);
	
	UFUNCTION(BlueprintGetter)
	FGuid GetTaskID() const { return ID; }
	
protected:
	UFUNCTION(BlueprintNativeEvent)
	void Execute(AActor* InExecutor);

public:
	UPROPERTY(BlueprintAssignable)
	FTaskQueueEvent OnTaskStarted;

	UPROPERTY(BlueprintAssignable)
	FTaskQueueEvent OnTaskFinished;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintGetter=GetTaskID)
	FGuid ID;
};
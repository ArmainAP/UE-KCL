// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/EventBusDataAsset.h"
#include "DelayedEventBusDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class GLOBALEVENTBUS_API UDelayedEventBusDataAsset : public UEventBusDataAsset
{
	GENERATED_BODY()

public:
	virtual void StartExecution_Implementation(AActor* InExecutor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float ExecutionDelay = 0;
	
protected:
	FTimerHandle TimerHandle;
};

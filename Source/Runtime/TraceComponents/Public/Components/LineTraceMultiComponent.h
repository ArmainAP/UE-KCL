// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/TraceComponent.h"
#include "LineTraceMultiComponent.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup=(Trace), meta=(BlueprintSpawnableComponent))
class TRACECOMPONENTS_API ULineTraceMultiComponent : public UTraceComponent
{
	GENERATED_BODY()

protected:
	virtual bool PerformTrace_Implementation(TArray<FHitResult>& OutHitResults) override;
};

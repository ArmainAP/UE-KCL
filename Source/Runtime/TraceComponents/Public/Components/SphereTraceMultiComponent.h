// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/TraceComponent.h"
#include "SphereTraceMultiComponent.generated.h"

/**
 * 
 */
UCLASS()
class TRACECOMPONENTS_API USphereTraceMultiComponent : public UTraceComponent
{
	GENERATED_BODY()

protected:
	virtual bool PerformTrace_Implementation(TArray<FHitResult>& OutHitResults) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trace")
	float Radius = 100.0f;
};

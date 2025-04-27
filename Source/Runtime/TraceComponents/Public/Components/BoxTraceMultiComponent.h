// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/TraceComponent.h"
#include "BoxTraceMultiComponent.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup=(Trace), meta=(BlueprintSpawnableComponent))
class TRACECOMPONENTS_API UBoxTraceMultiComponent : public UTraceComponent
{
	GENERATED_BODY()

	virtual bool PerformTrace_Implementation(TArray<FHitResult>& OutHitResults) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trace|Box")
	FVector HalfSize = FVector::OneVector;
};

// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "TracedActorNotify.generated.h"

class UTraceComponent;

UINTERFACE(Blueprintable, BlueprintType)
class TRACECOMPONENTS_API UTracedActorNotify : public UInterface
{
	GENERATED_BODY()
};

class TRACECOMPONENTS_API ITracedActorNotify
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void TraceStart(const UTraceComponent* TraceComponent, const FHitResult& HitResult);

	UFUNCTION(BlueprintNativeEvent)
	void TraceEnd(const UTraceComponent* TraceComponent, const FHitResult& HitResult);
};

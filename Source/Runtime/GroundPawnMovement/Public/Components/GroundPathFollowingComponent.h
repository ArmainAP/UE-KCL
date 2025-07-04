// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"
#include "GroundPathFollowingComponent.generated.h"

/**
 * 
 */
UCLASS()
class GROUNDPAWNMOVEMENT_API UGroundPathFollowingComponent : public UPathFollowingComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Push(FVector PushVector);
	virtual void Push_Implementation(FVector PushVector) { };
};

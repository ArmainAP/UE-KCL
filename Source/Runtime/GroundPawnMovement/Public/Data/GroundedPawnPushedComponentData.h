// Copyright Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structs/GroundPawnPush.h"
#include "GroundedPawnPushedComponentData.generated.h"

/**
 * 
 */
UCLASS()
class GROUNDPAWNMOVEMENT_API UGroundedPawnPushedComponentData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGroundPawnPush PushSettings;
};

// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/EngineTypes.h"
#include "InputInteractionDataAsset.generated.h"

class UInputAction;

UCLASS(Blueprintable, BlueprintType)
class INPUTINTERACTION_API UInputInteractableDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Detection")
	float MaxSelectionDistanceFromCharacter = 350.f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Detection")
	float MaxSelectionDistanceFromScreenCenter = 50.f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Detection")
	float MaxDetectionDistance = 700.f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Detection")
	bool bDetectWhenObstructed = false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Detection")
	bool bAllowInteractionWhenObstructed = false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Detection")
	TEnumAsByte<ECollisionChannel> ObstructionTraceChannel = ECC_Visibility;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Interaction")
	bool bIsHoldInteraction = true;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Interaction")
	float InteractionDuration = 1.f;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Interaction")
	float DelayBetweenInteraction = 0.5f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction Key")
	TObjectPtr<UInputAction> InputAction;
};

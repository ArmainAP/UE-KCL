// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/FormationDataAsset.h"
#include "WedgeFormationDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class FORMATIONSYSTEM_API UWedgeFormationDataAsset : public UFormationDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Angle = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Padding = 180.0f;

	virtual void GetOffsetTransforms_Implementation(const TArray<AActor*>& Actors, TArray<FTransform>& OutTransforms) override;
};

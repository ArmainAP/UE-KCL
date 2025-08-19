// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FormationDataAsset.h"
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

	virtual FTransform GetOffsetTransformForIndex_Implementation(const int Index, const int UnitCount) const override;
};

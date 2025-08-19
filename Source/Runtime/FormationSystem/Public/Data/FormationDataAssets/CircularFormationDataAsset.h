// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FormationDataAsset.h"
#include "CircularFormationDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class FORMATIONSYSTEM_API UCircularFormationDataAsset : public UFormationDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartAngle = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Degrees = 360.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius = 180.0f;

	UPROPERTY(Transient, VisibleAnywhere)
	mutable float Segments;

	virtual FTransform GetOffsetTransformForIndex_Implementation(const int Index, const int UnitCount) const override;
};
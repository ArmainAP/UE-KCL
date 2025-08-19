// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FormationDataAsset.h"
#include "RowFormationDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class FORMATIONSYSTEM_API URowFormationDataAsset : public UFormationDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxColumns = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Padding = FVector2D(200.0f, 200.0f);

	virtual FTransform GetOffsetTransformForIndex_Implementation(const int Index, const int UnitCount) const override;
};

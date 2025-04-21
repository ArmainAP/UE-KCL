// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FormationDataAsset.h"
#include "SquareFormationDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class FORMATIONSYSTEM_API USquareFormationDataAsset : public UFormationDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Padding = 200.0f;

	virtual void GetOffsetTransforms_Implementation(const int UnitCount, TArray<FTransform>& OutTransforms) override;
};

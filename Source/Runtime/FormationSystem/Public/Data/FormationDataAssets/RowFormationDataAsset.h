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

	FVector2D Padding = FVector2D(200.0f, 200.0f);

	UFUNCTION(BlueprintPure, BlueprintCallable)
	int GetTotalRows(const int ObjectCount) const;

	virtual void GetOffsetTransforms_Implementation(const int UnitCount, TArray<FTransform>& OutTransforms) override;
};

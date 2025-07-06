// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"
#include "Structs/GroundPathFollowingSettings.h"
#include "Structs/GroundPathFollowingSineWave.h"
#include "Structs/GroundPathFollowingSpeedVariation.h"
#include "Structs/GroundPathTrajectory.h"
#include "GroundPathFollowingComponent.generated.h"

class USplineComponent;
class UGroundedPathFollowingData;
/**
 * 
 */
UCLASS()
class GROUNDPAWNMOVEMENT_API UGroundPathFollowingComponent : public UPathFollowingComponent
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

public:
	virtual void BeginPlay() override;
	virtual FVector GetMoveFocus(bool bAllowStrafe) const override;
	virtual void UpdateCachedComponents() override;
	virtual void FollowPathSegment(float DeltaTime) override;

	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Push(FVector PushVector);
	virtual void Push_Implementation(FVector PushVector) { };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGroundPathFollowingSettings PathFollowingSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGroundPathTrajectory Trajectory;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGroundPathFollowingSpeedVariation SpeedVariation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGroundPathFollowingSineWave SineWave;

protected:
	bool ShouldDecelerate() const;
	bool IsNavmeshBlocked() const;
	void TriggerRepath() const;
	virtual void ExecuteFollowPathSegment(float DeltaTime) {};

	UPROPERTY(Transient)
	TObjectPtr<USplineComponent> SplineComponent;

	UPROPERTY(Transient)
	FVector FocusPoint = FVector::ZeroVector;

	UPROPERTY(Transient)
	FVector OldSegmentDirection = FVector::ZeroVector;

	UPROPERTY(Transient)
	bool ForceRegenerateSplineRepath = false;
};

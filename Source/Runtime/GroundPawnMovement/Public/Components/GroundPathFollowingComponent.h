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
	virtual void OnPathFinished(const FPathFollowingResult& Result) override;
	virtual void SetMoveSegment(int32 SegmentStartIndex) override;

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
	FVector GetPointOnSpline(const float Distance, const float OverrideZ) const;
	bool ShouldRegenerateSpline(const FVector& InPreviousTarget, const FVector& InCurrentTarget);
	void UpdateFocusPoint(float DeltaTime, const FVector& CurrentLocation, const FVector& XYVelocity);
	void RegenerateSpline(int32 SegmentStartIndex);
	void GenerateSplineForCurrentSegment(int32 SegmentStartIndex, float TangentScaleMultiplier = 1.0f);
	TOptional<float> FindValidCurvatureReduction(const int32 SegmentStartIndex, const float MaxReduction = 1.0f, const int32 Steps = 10, const float DistanceCheck = 100.0f);
	bool IsSplinePathValid(const float DistanceIncrement) const;

	UPROPERTY(Transient)
	TObjectPtr<USplineComponent> SplineComponent;

	UPROPERTY(Transient)
	FVector FocusPoint = FVector::ZeroVector;

	UPROPERTY(Transient)
	FVector OldSegmentDirection = FVector::ZeroVector;

	UPROPERTY(Transient)
	bool ForceRegenerateSplineRepath = false;

	UPROPERTY(Transient)
	float CurrentDistanceOnSpline = 0.0f;

	UPROPERTY(Transient)
	FVector LastValidEndPointSpline;

	UPROPERTY(Transient)
	float CurrentTrajectoryCurvatureReduction = 1.0f;

	UPROPERTY(Transient)
	FVector CurrentMoveVector = FVector::ZeroVector;

	UPROPERTY(Transient)
	FVector OldSecondTangent = FVector::ZeroVector;

	UPROPERTY(Transient)
	FVector TargetFocusPoint = FVector::ZeroVector;

	UPROPERTY(Transient)
	FVector SplineTargetPosition = FVector::ZeroVector;
};

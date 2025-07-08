// Copyright Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveFloat.h"
#include "GroundPathFollowingSettings.generated.h"

/**
 * Settings controlling ground‑spline path‑following behaviour for AI pawns.
 * All distances are expressed in centimetres, angles in degrees and speeds in centimetres/second.
 */
USTRUCT(BlueprintType)
struct GROUNDPAWNMOVEMENT_API FGroundPathFollowingSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bDebug = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool OrientToImpulse = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool EndEarlyIfEndGoalIsBlocked = false;
    
    /* ---------- Spline progression ------------------------------------------------------ */

    /** If the closest point on spline is farther than this value, skip to that point. */
    UPROPERTY(EditAnywhere, Category="Path Following|Spline", meta=(ClampMin="0.0"))
    float MinDistanceSkip = 50.f;

    /** Maximum look‑ahead distance when the pawn is blocked. */
    UPROPERTY(EditAnywhere, Category="Path Following|Spline", meta=(ClampMin="0.0"))
    float MaxDistanceProgressOnSplineWhenBlocked = 300.f;

    /** Radius within which the current spline target is considered reached. */
    UPROPERTY(EditAnywhere, Category="Path Following|Spline", meta=(ClampMin="0.0"))
    float ReachRadiusOnSpline = 60.f;

    /** Distance added along the spline whenever we reach the current target. */
    UPROPERTY(EditAnywhere, Category="Path Following|Spline", meta=(ClampMin="0.0"))
    float DistanceIncrementSpline = 900.f;

    /** Extra distance added to @ref DistanceIncrementSpline at full speed (time‑dilation safe). */
    UPROPERTY(EditAnywhere, Category="Path Following|Spline", meta=(ClampMin="0.0"))
    float VelocityBoostDistanceIncrementSpline = 900.f;

    /** Minimum tangent length enforced on the spline. */
    UPROPERTY(EditAnywhere, Category="Path Following|Spline", meta=(ClampMin="0.0"))
    float MinTangentLength = 50.f;

    /** Generate a new path if vertical gap to target exceeds this threshold. */
    UPROPERTY(EditAnywhere, Category="Path Following|Spline", meta=(ClampMin="0.0"))
    float TargetHeightThresholdRegeneratePath = 250.f;

    /* ---------- Speed modifiers --------------------------------------------------------- */

    /** Slow‑down factor applied at maximum turn angle (0 keeps speed, 1 stops). */
    UPROPERTY(EditAnywhere, Category="Path Following|Speed", meta=(ClampMin="0.0", ClampMax="1.0"))
    float SlowDownTurnAmount = 0.18f;

    /** Applied when @p YawDifference reaches this value. */
    UPROPERTY(EditAnywhere, Category="Path Following|Speed", meta=(ClampMin="0.0"))
    float MaxAngleNormalizeSlowDownTurn = 180.f;

    /** Lower bound for slow‑down when avoiding other pawns (0‑1). */
    UPROPERTY(EditAnywhere, Category="Path Following|Speed|Avoidance", meta=(ClampMin="0.0", ClampMax="1.0"))
    float MinSlowDownPawnAvoidance = 0.1f;

    /** Upper bound for slow‑down when avoiding other pawns (0‑1). */
    UPROPERTY(EditAnywhere, Category="Path Following|Speed|Avoidance", meta=(ClampMin="0.0", ClampMax="1.0"))
    float MaxSlowDownPawnAvoidance = 0.5f;

    /** End‑path braking distance expressed as a fraction of MaxSpeed. */
    UPROPERTY(EditAnywhere, Category="Path Following|Speed", meta=(ClampMin="0.0"))
    double EndPathDecelerationMultiplier = 0.3;

    /** Desired‑move vector lerp speed: higher values snap faster to the path. */
    UPROPERTY(EditAnywhere, Category="Path Following|Speed", meta=(ClampMin="0.0"))
    float MoveVectorInterpSpeed = 8.f;

    /* ---------- Curves ------------------------------------------------------------------ */

    /** Rotation‑rate multiplier as a function of pawn speed. (X = speed, Y = multiplier) */
    UPROPERTY(EditAnywhere, Category="Path Following|Curves")
    FRuntimeFloatCurve RotationRateToSpeedRatio;

    /** Terrain pitch to speed multiplier. (X = pitch°, Y = speed multiplier) */
    UPROPERTY(EditAnywhere, Category="Path Following|Curves")
    FRuntimeFloatCurve TerrainAngleToSpeedMultiplier;

    /* ---------- Helpers ----------------------------------------------------------------- */

    /** True when @p Tangent is shorter than @ref MinTangentLength. */
    FORCEINLINE bool HasMinTangentLength(const FVector& Tangent) const
    {
        return Tangent.Length() < MinTangentLength;
    }

    /** Evaluate rotation‑rate multiplier for the given speed. */
    FORCEINLINE float GetRotationRateToSpeedRatio(float Speed) const
    {
        return RotationRateToSpeedRatio.GetRichCurveConst()->Eval(Speed);
    }

    /** Randomised avoidance slow‑down factor between configured bounds. */
    FORCEINLINE float GetRandomSlowDownPawnAvoidance() const
    {
        return FMath::RandRange(MinSlowDownPawnAvoidance, MaxSlowDownPawnAvoidance);
    }

    /**
     * Composite slow‑down ratio accounting for turning and avoidance.
     * @param AvoidanceWeight Weight in the [0‑1] range.
     * @param YawDifference   Absolute yaw delta in degrees.
     */
    FORCEINLINE float GetSlowDownTurnRatio(float AvoidanceWeight, float YawDifference) const
    {
        const float TurnRatio = FMath::Clamp(FMath::Abs(YawDifference) / MaxAngleNormalizeSlowDownTurn, 0.f, 1.f);
        const float SlowDown  = TurnRatio * SlowDownTurnAmount + AvoidanceWeight * GetRandomSlowDownPawnAvoidance();
        return FMath::Clamp(1.f - SlowDown, 0.f, 1.f);
    }

    /** Evaluate terrain‑pitch speed multiplier (positive pitch = uphill). */
    FORCEINLINE float GetTerrainAngleToSpeedMultiplier(float PitchTerrainAngle) const
    {
        return TerrainAngleToSpeedMultiplier.GetRichCurveConst()->Eval(-PitchTerrainAngle);
    }

    /** Determine whether the vertical gap warrants path regeneration. */
    FORCEINLINE bool IsOverTargetHeight(float Height) const
    {
        return Height > TargetHeightThresholdRegeneratePath;
    }

    /** Distance to advance along the spline this tick. */
    FORCEINLINE float IncrementDistance(float CurrentNormalizedVelocity, float DeltaTime) const
    {
        return (DistanceIncrementSpline + CurrentNormalizedVelocity * VelocityBoostDistanceIncrementSpline) * DeltaTime;
    }

    /** Can we skip the current spline point? */
    FORCEINLINE bool CanSkip(float Distance) const
    {
        return Distance > MinDistanceSkip;
    }

    /** Has the pawn not reached the current target point yet? */
    FORCEINLINE bool HasNotReached(float Distance) const
    {
        return Distance < ReachRadiusOnSpline;
    }

    /** Are we allowed to progress while blocked? */
    FORCEINLINE bool CanProgress(float Distance) const
    {
        return Distance < MaxDistanceProgressOnSplineWhenBlocked;
    }
};

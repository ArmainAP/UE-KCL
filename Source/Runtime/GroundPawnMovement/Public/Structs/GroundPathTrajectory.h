// Copyright Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveFloat.h"
#include "GroundPathTrajectory.generated.h"

/**
 * Compact data‑container describing how a ground‑moving pawn bends its spline trajectory
 * when navigating towards a (possibly moving) target.  All tweakable knobs are exposed
 * to designers via Blueprint, while helper functions expose the math that transforms
 * those knobs into runtime scalars the movement component can apply.
 */
USTRUCT(BlueprintType)
struct GROUNDPAWNMOVEMENT_API FGroundPathTrajectory
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bDebug = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(UIMin="1"))
    float CurvatureBooster = 800.0f;
    
    /* ============================== Trajectory Regeneration ============================== */

    /** Distance (cm) the end‑point may drift before we regenerate the current spline segment. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trajectory|Regeneration", meta=(ClampMin="0.0", UIMin="0.0", ForceUnits="cm"))
    float RadiusEndPointRegenerateSpline = 600.f;

    /** If true, a fresh spline is rebuilt every tick regardless of drift distance. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trajectory|Regeneration")
    bool bAlwaysRegenerateSplines = false;
    
    /* ============================== Curvature Weights ============================== */

    /** Weight of the pawn’s previous heading when building an in‑tangent. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trajectory|Curvature", meta=(ClampMin="0.0", UIMin="0.0"))
    float OldDirCurveInfluence = 1.f;

    /** Weight of the vector pointing straight at the target when it lies behind the pawn. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trajectory|Curvature", meta=(ClampMin="0.0", UIMin="0.0"))
    float OppositeDirCurveInfluence = 1.f;

    /**
     * X‑axis: dot product between current forward and (Target − Pawn) ∈ [‑1,1].
     * Y‑axis: blend α where 0 => use PreviousDir, 1 => use OppositeDir for the first tangent.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trajectory|Curvature")
    FRuntimeFloatCurve OppositeSideInfluencePerDotProduct;
    
    /* ============================== Tangent Length Modifiers ============================== */

    /** Maximum segment length (cm) after which distance no longer alters curvature. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trajectory|Tangent", meta=(ClampMin="0.0", UIMin="0.0", ForceUnits="cm"))
    float MaxDistanceInfluenceCurvature = 1000.f;

    /** Segments shorter than this length are forced straight (no curvature). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trajectory|Tangent", meta=(ClampMin="0.0", UIMin="0.0", ForceUnits="cm"))
    float MinSegmentLengthCurvature = 0.f;

    /** Scalar applied to the out‑tangent of the spline segment. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trajectory|Tangent", meta=(ClampMin="0.0", UIMin="0.0"))
    float SecondTangentMultiplier = 0.65f;

    /** Maps normalised pawn speed (0..1) → tangent scale multiplier. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trajectory|Tangent")
    FRuntimeFloatCurve TangentInfluenceToNormalizedSpeed;

    /** Maps normalised segment length (0..1) → tangent scale multiplier. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trajectory|Tangent")
    FRuntimeFloatCurve TangentInfluenceToSplineLength;
    
    /* ============================== Inline Utility ============================== */

    /** Returns a 0‑1 scalar derived from the raw segment distance (cm). */
    FORCEINLINE float GetDistanceInfluence(const float SegmentDistance) const
    {
        const float Normalised = FMath::Clamp(SegmentDistance / MaxDistanceInfluenceCurvature, 0.f, 1.f);
        return TangentInfluenceToSplineLength.GetRichCurveConst()->Eval(Normalised);
    }

    /** Returns a 0‑1 scalar derived from the pawn’s normalised speed (0‑1). */
    FORCEINLINE float GetSpeedInfluence(const float NormalisedSpeed) const
    {
        return TangentInfluenceToNormalizedSpeed.GetRichCurveConst()->Eval(FMath::Clamp(NormalisedSpeed, 0.f, 1.f));
    }

    /** Returns α in [0,1] blending PreviousDir (0) vs OppositeDir (1). */
    FORCEINLINE float GetOppositeBlend(const float DotOldVsTarget) const
    {
        const float Alpha = FMath::Clamp(-DotOldVsTarget, 0.f, 1.f);      // full opposite when dot == ‑1
        return OppositeSideInfluencePerDotProduct.GetRichCurveConst()->Eval(Alpha);
    }

    /** Indicates whether the given segment length should be curved. */
    void CheckCurvature(const float SegmentLength)
    {
        bHasMinLengthCurvature = SegmentLength >= MinSegmentLengthCurvature;
    }

    /** Return true when the spline must be rebuilt for the current target. */
    FORCEINLINE bool ShouldRegenerate(const FVector& CurrentTargetLocal, const FVector& PreviousTarget, const FVector& ClosestPointOnSpline, const FVector& LastValidEndPointSpline)
    {
        if (bAlwaysRegenerateSplines) return true;
        CheckCurvature(FVector::Dist(CurrentTargetLocal, PreviousTarget));
        if (!bHasMinLengthCurvature) return true;
        if (FVector::Dist(ClosestPointOnSpline, CurrentTargetLocal) > RadiusEndPointRegenerateSpline) return true;
        if (FVector::Dist(LastValidEndPointSpline, CurrentTargetLocal) > RadiusEndPointRegenerateSpline) return true;
        return false;
    }

    FORCEINLINE bool HasMinLengthCurvature() const
    {
        return bHasMinLengthCurvature;
    }

protected:
    bool bHasMinLengthCurvature = false;
};

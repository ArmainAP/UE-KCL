// Copyright to Kat Code Labs, SRL. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveFloat.h"
#include "GroundPathFollowingSineWave.generated.h"

// -----------------------------  Context ------------------------------------
USTRUCT(BlueprintType)
struct FSplineSample
{
    GENERATED_BODY()

    /** Distance travelled along spline (uu). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spline")
    float Distance = 0.f;

    /** Total spline length (uu). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spline")
    float SplineLength = 0.f;

    /** Lateral axis at the sample point. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spline")
    FVector Right = FVector::RightVector;
};

USTRUCT(BlueprintType)
struct FMoveInfluence
{
    GENERATED_BODY()

    /** Pawn speed mapped to [0‑1] range. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
    float NormalisedSpeed = 0.f;

    /** Random tempo multiplier (usually 0.8‑1.2). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
    float SpeedVariation = 1.f;

    /** Factor that damps zig‑zag on tight curves (0‑1). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
    float CurvatureReduction = 1.f;
};

// ---------------------------  Sine wave ------------------------------------
USTRUCT(BlueprintType)
struct GROUNDPAWNMOVEMENT_API FGroundPathFollowingSineWave
{
    GENERATED_BODY()
    /* -------------------- Tunables -------------------- */

    /** Enables sine‑wave offset generation. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SineWave")
    bool bEnable = false;

    /** Minimum lateral offset intensity (uu). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.0"), Category="SineWave|Intensity")
    float MinIntensity = 40.f;

    /** Maximum lateral offset intensity (uu). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.0"), Category="SineWave|Intensity")
    float MaxIntensity = 200.f;

    /** Distance (uu) between successive peaks. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="1.0"), Category="SineWave")
    float Wavelength = 100.f;

    /** Normalised‑speed (x) → intensity multiplier (y). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SineWave|Intensity")
    FRuntimeFloatCurve InfluenceToNormalizedSpeed;

    /** Distance (uu) near each end of the spline where offset fades. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.0"), Category="SineWave|Fade")
    float SmoothDistance = 200.f;

    /** Fade‑in/out shaping curve (x:[0‑1] y:weight). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SineWave|Fade")
    FRuntimeFloatCurve SmoothingCurve;

    /* -------------------- Runtime --------------------- */

    /** Call once per nav segment to randomise intensity. */
    FORCEINLINE void UpdateSegmentIntensity()
    {
        CurrentSegmentIntensity = FMath::FRandRange(MinIntensity, MaxIntensity);
    }

    /** Adds lateral displacement to the provided point. */
    FORCEINLINE void ApplyToPoint(
        FVector&              Point,
        const FSplineSample&  Spline,
        const FMoveInfluence& Inf) const
    {
        if (!bEnable || FMath::IsNearlyZero(CurrentSegmentIntensity)) { return; }

        const float IntensitySmoothing = EvaluateSmoothing(Spline, Inf);
        if (FMath::IsNearlyZero(IntensitySmoothing) || FMath::IsNearlyZero(Inf.CurvatureReduction)) { return; }

        const float Phase       = Spline.Distance / Wavelength;
        const float Displacement = FMath::Sin(Phase) * CurrentSegmentIntensity * IntensitySmoothing * FMath::Abs(Inf.CurvatureReduction);
        Point += Spline.Right * Displacement;
    }

    /** Adjusts spline tangent to include sine‑wave derivative. */
    FORCEINLINE void ApplyToTangent(
        FVector&              Tangent,
        const FSplineSample&  Spline,
        const FMoveInfluence& Inf) const
    {
        if (!bEnable || FMath::IsNearlyZero(CurrentSegmentIntensity)) { return; }

        const float IntensitySmoothing = EvaluateSmoothing(Spline, Inf);
        if (FMath::IsNearlyZero(IntensitySmoothing)) { return; }

        const float Phase          = Spline.Distance / Wavelength;
        const float WaveDerivative = CurrentSegmentIntensity * IntensitySmoothing * FMath::Cos(Phase) / Wavelength;
        Tangent += Spline.Right * WaveDerivative;
    }

private:
    /* -------- Cached per‑segment amplitude -------- */
    float CurrentSegmentIntensity = 0.f;

    /* ------------- Helper evaluations -------------- */
    FORCEINLINE float EvaluateInfluence(const FMoveInfluence& Inf) const
    {
        return InfluenceToNormalizedSpeed.GetRichCurveConst()->Eval(Inf.NormalisedSpeed) * Inf.SpeedVariation;
    }

    FORCEINLINE float EvaluateSmoothing(const FSplineSample& Spline, const FMoveInfluence& Inf) const
    {
        const float Influence = EvaluateInfluence(Inf);

        if (const float HalfLen = Spline.SplineLength * 0.5f;
            HalfLen <= SmoothDistance) { return 0.f; } // Entire spline is inside fade zones

        // Entrance fade‑in
        if (Spline.Distance < SmoothDistance)
        {
            const float Alpha = Spline.Distance / SmoothDistance;
            return SmoothingCurve.GetRichCurveConst()->Eval(Alpha) * Influence;
        }

        // Exit fade‑out
        if (Spline.Distance > Spline.SplineLength - SmoothDistance)
        {
            const float Alpha = 1.f - (Spline.Distance - (Spline.SplineLength - SmoothDistance)) / SmoothDistance;
            return SmoothingCurve.GetRichCurveConst()->Eval(Alpha) * Influence;
        }

        // Mid‑section constant weight
        return Influence;
    }
};

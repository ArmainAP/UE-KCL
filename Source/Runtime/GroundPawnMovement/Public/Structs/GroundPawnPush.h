// Copyright Kat Code Labs, SRL. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GroundPawnPush.generated.h"

/**
 * Lightweight settings container controlling how a pawn can be pushed and
 * how it pushes other pawns. All tunables are exposed to Blueprints and can
 * safely be edited per–instance.
 *
 *  * Booleans are packed into a single byte to minimise memory cost when used
 *    by hundreds of AIs.
 *  * Semantic naming clarifies the intent of each field.
 *  * Convenience inlines already account for the gating flags.
 */
USTRUCT(BlueprintType)
struct FGroundPawnPush
{
    GENERATED_BODY()

    /* -------------- Oscillation ------------- */

    /** Max percentage (0‑1) by which the oscillation can attenuate the incoming push force. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push|Oscillation", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MaxOscillationForcePct = 0.2f;

    /** Duration, in seconds, of a full oscillation cycle. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push|Oscillation", meta = (ClampMin = "0.0"))
    float OscillationPeriod = 3.f;

    /* --------------- Permissions -------------- */

    /** Whether the pawn can be pushed while it is moving. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push|Permissions")
    uint8 bCanBePushedWhileMoving : 1;

    /** Whether the pawn can be pushed while it is stationary. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push|Permissions")
    uint8 bCanBePushedWhileStationary : 1;

    /** Whether the pawn transfers force to other pawns on collision. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push|Permissions")
    uint8 bCanPushOthers : 1;

    /* -------------- Speed limits -------------- */

    /** Speed multiplier applied when the pawn is being pushed and itself moving. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push|Speed", meta = (ClampMin = "0.0", ClampMax = "25.0"))
    float PushSpeedFactorMoving = 1.2f;

    /** Speed multiplier applied when the pawn is being pushed while stationary. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push|Speed", meta = (ClampMin = "0.0", ClampMax = "25.0"))
    float PushSpeedFactorStationary = 8.0f;

    /* ------------- Active pushing ------------- */

    /** Base force used when this pawn pushes other pawns. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push|Active", meta = (ClampMin = "0.0"))
    float ActivePushForce = 1200.f;

    /** Fraction (0‑1) of the incoming push force that gets propagated to subsequent pawns. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push|Active", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float PushForceTransferPct = 0.2f;

    /* ---------------- Helpers ---------------- */

    /** Returns the effective speed factor when stationary, gated by permissions. */
    FORCEINLINE float GetStationarySpeedFactor() const
    {
        return bCanBePushedWhileStationary ? PushSpeedFactorStationary : 0.f;
    }

    /** Returns the effective speed factor when moving, gated by permissions. */
    FORCEINLINE float GetMovingSpeedFactor() const
    {
        return bCanBePushedWhileMoving ? PushSpeedFactorMoving : 0.f;
    }

    /** Default constructor sets all permission flags to true. */
    FGroundPawnPush()
        : bCanBePushedWhileMoving(true)
        , bCanBePushedWhileStationary(true)
        , bCanPushOthers(true)
    {}
};

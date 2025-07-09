// Copyright Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveFloat.h"
#include "GroundPawnSensor.generated.h"

/**
 * Lightweight runtime representation of a single sensor hit.
 * Contains information required by avoidance and wall–following logic.
 */
USTRUCT(BlueprintType)
struct GROUNDPAWNMOVEMENT_API FSensorCollisionData
{
	GENERATED_BODY()

	/** Whether the trace hit something. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCollided = false;

	/** Distance from sensor origin to the blocking hit (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.0"))
	float CollisionDistance = 0.0f;

	/** Local direction of the sensor shot. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SensorDirection = FVector::ZeroVector;

	/** Normal of the blocking hit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CollisionNormal = FVector::ZeroVector;

	/** Collision channel used for the blocking hit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECollisionChannel> HitCollisionChannel = ECC_Visibility;

	/** Signed yaw angle (deg) relative to the pawn forward. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SensorAngle = 0.0f;

	/** Whether a primitive component was found on the hit actor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bComponentFound = false;

	/** Actor that was hit. nullptr if none. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> CollidedActor = nullptr;

	/** Returns a [0..1] weight inversely proportional to CollisionDistance. */
	FORCEINLINE float ComputeDistanceWeight(const float InMaxDistance) const
	{
		return 1.f - FMath::Clamp(CollisionDistance / InMaxDistance, 0.f, 1.f);
	}

	/** Returns a [0..1] weight inversely proportional to |SensorAngle|. */
	FORCEINLINE float ComputeAngleWeight(const float NormalizingAngle) const
	{
		return 1.f - FMath::Clamp(FMath::Abs(SensorAngle) / NormalizingAngle, 0.f, 1.f);
	}
};

/**
 * Collection of configurable parameters that drive avoidance and wall–following logic
 * for ground–based pawns.  All values are exposed to Blueprints to facilitate tuning.
 */
USTRUCT(BlueprintType)
struct GROUNDPAWNMOVEMENT_API FGroundPawnSensorSettings
{
	GENERATED_BODY()

	/** Toggle runtime sensing and avoidance. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General")
	bool bActive = true;

	/** Max camera distance (cm) at which sensing is performed in standalone builds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="General", meta=(ClampMin="0.0"))
	float MaxDistanceCameraSensing = 5000.f;

	/** Radius (cm) of the sphere trace for each sensor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sensors", meta=(ClampMin="1.0"))
	float SensorRadius = 10.f;

	/** Total horizontal FOV (deg) covered by all sensors. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sensors", meta=(ClampMin="0.0", ClampMax="360.0"))
	float MaxConeAngle = 260.f;

	/** Angle (deg) used to normalize SensorAngle when computing weights. Must exceed MaxConeAngle. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sensors", meta=(ClampMin="0.0", ClampMax="360.0"))
	float AngleNormalizationRange = 280.f;

	/** Number of rays / sphere traces. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sensors", meta=(ClampMin="1"))
	int32 TraceCount = 12;

	/** Max detection distance for sensor traces (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sensors", meta=(ClampMin="0.0"))
	float MaxSensorDistance = 120.f;

	/** Forward offset (cm) applied to sensor origin relative to pawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sensors")
	float ForwardOriginOffset = 10.f;

	/** Clip distance (cm) from sensor origin. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sensors")
	float NearClipping = 0.f;

	/** Upward offset (cm) applied to sensor origin. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sensors")
	float UpOriginOffset = 0.f;

	/** Collision channels that trigger wall–following. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behavior")
	TArray<TEnumAsByte<ECollisionChannel>> WallFollowingChannels;

	/** Collision channels that trigger avoidance. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behavior")
	TArray<TEnumAsByte<ECollisionChannel>> AvoidanceChannels;

	/** Actor classes that sensor traces ignore entirely. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behavior")
	TArray<TSubclassOf<AActor>> IgnoredActorClasses;

	/** Tags that sensor traces ignore entirely. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behavior")
	TArray<FName> IgnoredActorTags;

	/** If distance to goal < this value (cm) sensors are disabled. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behavior", meta=(ClampMin="0.0"))
	float StopSensingDistanceFromGoal = 100.f;

	/** Min distance (cm) to blocking obstacle before triggering wall–follow. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behavior", meta=(ClampMin="0.0"))
	float WallFollowDistance = 150.f;

	/** Draw debug geometry? (capsules, lines, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	bool bDebugDraw = false;

	/** Non‑linear scaling of angle weight. X‑axis expects ComputeAngleWeight [0..1]. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tuning")
	FRuntimeFloatCurve AngleWeightCurve;

	/** Non‑linear scaling of distance weight. X‑axis expects ComputeDistanceWeight [0..1]. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tuning")
	FRuntimeFloatCurve DistanceWeightCurve;

	/** Interp speed controlling smoothness of avoidance steering. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tuning", meta=(ClampMin="0.0"))
	float AvoidanceInterpSpeed = 18.f;

	/** Global multiplier for computed avoidance force. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tuning")
	float AvoidanceForceMultiplier = 1.f;

	FGroundPawnSensorSettings()
	{
		if (FRichCurve* RichCurve = AngleWeightCurve.GetRichCurve();
			RichCurve && RichCurve->GetNumKeys() == 0)
		{
			RichCurve->AddKey(0.f, 1.f);
		}

		if (FRichCurve* RichCurve = DistanceWeightCurve.GetRichCurve();
			RichCurve && RichCurve->GetNumKeys() == 0)
		{
			RichCurve->AddKey(0.f, 1.f);
		}
	}

	/** Returns angle influence weight scaled by configured curve. */
	FORCEINLINE float GetAngleFactor(const FSensorCollisionData& SensorData) const
	{
		const float NormalizedAngle = SensorData.ComputeAngleWeight(AngleNormalizationRange);
		return AngleWeightCurve.GetRichCurveConst()->Eval(NormalizedAngle);
	}

	/** Returns distance influence weight scaled by configured curve. */
	FORCEINLINE float GetDistanceFactor(const FSensorCollisionData& SensorData) const
	{
		const float NormalizedDistance = SensorData.ComputeDistanceWeight(MaxSensorDistance);
		return DistanceWeightCurve.GetRichCurveConst()->Eval(NormalizedDistance);
	}
};

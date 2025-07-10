// Copyright Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs/GroundPawnSensor.h"
#include "GroundedPawnAvoidanceSensing.generated.h"

class UGroundedPawnAvoidanceSensingData;
/**
 * Spherical‑sensor avoidance and wall‑following for ground‑based pawns.
 *
 * The component fires a configurable set of overlapping sphere traces arranged
 * in a horizontal cone.  Hit data are distilled into FSensorCollisionData,
 * weighted, and turned into steering vectors that modulate the AI move vector
 * supplied by the path‑following logic.
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class GROUNDPAWNMOVEMENT_API UGroundedPawnAvoidanceSensing : public UActorComponent
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	
public:
	/** Locates the sensing component attached to an Actor (if any). */
	UFUNCTION(BlueprintCallable)
	static UGroundedPawnAvoidanceSensing* Find(AActor* Actor);

	UGroundedPawnAvoidanceSensing();

	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface

	/** Temporarily pause / resume sensing. */
	UFUNCTION(BlueprintCallable)
	void SetTempStopSensing(bool bShouldStop);

	/** True if any sensor currently reports an obstacle or the goal is blocked. */
	UFUNCTION(BlueprintCallable)
	bool IsAnythingDetected() const;

	/** [0..1] slow‑down weight based only on other pawns in front. */
	UFUNCTION(BlueprintCallable)
	float GetPawnAvoidanceSlowDownWeight() const;

	/** Unit vector tangent to the obstacle the pawn is wall‑following. */
	UFUNCTION(BlueprintCallable)
	FVector GetWallFollowingDirection() const;

	/** Modifies the AI desired movement vector to avoid obstacles & walls. */
	UFUNCTION(BlueprintCallable)
	FVector ModifyAITrajectoryForAvoidance(const FVector& CurrentMoveVector, float DeltaTime, const FVector& CurrentTarget);

	UFUNCTION(BlueprintGetter)
	TArray<FSensorCollisionData> GetRecentHits() const { return RecentHits; }

	UFUNCTION(BlueprintGetter)
	bool IsWallFollowing() const { return bIsWallFollowing; }

	/** Returns angle influence weight scaled by configured curve. */
	UFUNCTION(BlueprintPure)
	float GetAngleFactor(const FSensorCollisionData& SensorData) const;

	/** Returns distance influence weight scaled by configured curve. */
	UFUNCTION(BlueprintPure)
	float GetDistanceFactor(const FSensorCollisionData& SensorData) const;

	UFUNCTION(BlueprintPure)
	float GetSensorScale() const;

	UFUNCTION(BlueprintPure)
	bool IsInSensingDistanceFromGoal(const float Distance) const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UGroundedPawnAvoidanceSensingData> PresetOverride;

protected:
	// Internal helpers
	bool ShouldSense() const;
	void UpdateSensors();
	void PerformSingleSensor(float AngleDeg, FSensorCollisionData& OutData) const;
	bool IsGoalReachable(const FVector& GoalLocation, FHitResult& OutHit) const;
	FVector ComputeAvoidanceDirection() const;

	/** Tunable parameters driving sensing behaviour. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGroundPawnSensorSettings Settings;
	
	/** Cached owning pawn for fast access & reduced casts. */
	UPROPERTY(Transient)
	TWeakObjectPtr<APawn> CachedPawn;

	/** Actors to ignore entirely during sensing. */
	UPROPERTY() // TODO: Add/Remove actors
	TArray<TObjectPtr<AActor>> IgnoredActors;

	/** Debug‑visible capsule sweep result of the current goal reach test. */
	UPROPERTY(BlueprintReadOnly)
	FHitResult ReachTestResult;

	/** True if the goal (CurrentTarget) is currently reachable (capsule sweep). */
	UPROPERTY(BlueprintReadOnly)
	bool bGoalReachable = true;

	/** Smoothed avoidance vector for frame‑to‑frame continuity. */
	FVector SmoothedAvoidanceVector = FVector::ZeroVector;

	/** True if clockwise tangent is shorter path during current wall‑follow. */
	bool bClockwiseWallFollow = true;

	/** Latest collision data for all sensors that reported a hit in the last tick. */
	UPROPERTY(BlueprintReadOnly, BlueprintGetter=GetRecentHits)
	TArray<FSensorCollisionData> RecentHits;

	/** True while the pawn follows an obstacle tangent instead of normal avoidance. */
	UPROPERTY(BlueprintReadOnly, BlueprintGetter=IsWallFollowing)
	bool bIsWallFollowing = false;
};
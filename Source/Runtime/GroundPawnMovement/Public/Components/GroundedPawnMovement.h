// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GroundedPawnMovement.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FPendingForces
{
	GENERATED_BODY()

	/** Accumulated impulse to be added next tick. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ImpulseToApply = FVector::ZeroVector;

	/** Accumulated force to be added next tick. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ForceToApply = FVector::ZeroVector;
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class GROUNDPAWNMOVEMENT_API UGroundedPawnMovement : public UFloatingPawnMovement
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

public:
	UGroundedPawnMovement();

	/** 
	* Add impulse to pawn. Impulses are accumulated each tick and applied together
	* so multiple calls to this function will accumulate.
	* An impulse is an instantaneous force, usually applied once. If you want to continually apply
	* forces each frame, use AddForce().
	* Note that changing the momentum of pawns like this can change the movement mode.
	* 
	* @param	Impulse				Impulse to apply.
	* @param	bVelocityChange		Whether or not the impulse is relative to mass.
	*/
	UFUNCTION(BlueprintCallable, Category="Pawn|Components|Movement")
	virtual void AddImpulse(FVector Impulse, bool bVelocityChange = false);

	/** 
	 * Add force to pawn. Forces are accumulated each tick and applied together
	 * so multiple calls to this function will accumulate.
	 * Forces are scaled depending on timestep, so they can be applied each frame. If you want an
	 * instantaneous force, use AddImpulse.
	 * Adding a force always takes the actor's mass into account.
	 * Note that changing the momentum of pawn like this can change the movement mode.
	 * 
	 * @param	Force			Force to apply.
	 */
	UFUNCTION(BlueprintCallable, Category="Pawn|Components|Movement")
	virtual void AddForce(FVector Force);

	/** Clears forces accumulated through AddImpulse() and AddForce(), and also pending launch velocity. */
	UFUNCTION(BlueprintCallable, Category="Pawn|Components|Movement")
	virtual void ClearAccumulatedForces();

	/** Returns the current gravity direction. */
	UFUNCTION(BlueprintGetter, Category="Pawn|Components|Movement")
	FVector GetGravityDirection() const { return GravityDirection; }

	/** Return true if the hit result should be considered a walkable surface for the character. */
	UFUNCTION(BlueprintCallable, Category="Pawn|Components|Movement")
	virtual bool IsWalkable(const FHitResult& Hit) const;

	/** Get the max angle in degrees of a walkable surface for the character. */
	UFUNCTION(BlueprintGetter, Category="Pawn|Components|Movement")
	float GetWalkableFloorAngle() const { return WalkableFloorAngle; }

	/** Set the max angle in degrees of a walkable surface for the character. Also computes WalkableFloorZ. */
	UFUNCTION(BlueprintSetter, Category="Pawn|Components|Movement")
	void SetWalkableFloorAngle(float InWalkableFloorAngle);
	
	/** Get the Z component of the normal of the steepest walkable surface for the character. Any lower than this and it is not walkable. */
	UFUNCTION(BlueprintGetter, Category="Pawn|Components|Movement")
	float GetWalkableFloorZ() const { return WalkableFloorZ; }

	/** Set the Z component of the normal of the steepest walkable surface for the character. Also computes WalkableFloorAngle. */
	UFUNCTION(BlueprintSetter, Category="Pawn|Components|Movement")
	void SetWalkableFloorZ(float InWalkableFloorZ);
	
	UFUNCTION(BlueprintPure)
	bool CanMove() const;
	
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RotateTowardsMovement(float DeltaTime) const;

	/** Applies momentum accumulated through AddImpulse() and AddForce(), then clears those forces. Does *not* use ClearAccumulatedForces() since that would clear pending launch velocity as well. */
	virtual void ApplyAccumulatedForces(float DeltaSeconds);

	/** Returns the size of a vector in the gravity-space vertical direction. */
	FVector::FReal GetGravitySpaceZ(const FVector& Vector) const { return Vector.Dot(-GetGravityDirection()); }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rotation")
	bool bRotateTowardMovement = true;
	
	/** Degrees-per-second interpolation speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rotation", meta=(EditCondition="bRotateTowardMovement"))
	float RotationSpeed = 720.f;

	/** Minimum horizontal speed before we start to rotate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rotation", meta=(EditCondition="bRotateTowardMovement"))
	float MinOrientSpeed = 10.f;

	/**
	* A normalized vector representing the direction of gravity for gravity relative movement modes: walking, falling,
	* and custom movement modes. Gravity direction remaps player input as being within the plane defined by the gravity
	* direction. Movement simulation values like velocity and acceleration are maintained in their existing world coordinate
	* space but are transformed internally as gravity relative (for instance moving forward up a vertical wall that gravity is
	* defined to be perpendicular to and jump "up" from that wall). If ShouldRemainVertical() is true the character's capsule
	* will be oriented to align with the gravity direction.
	*/
	UPROPERTY(Category="Movement: Gravity", EditAnywhere, BlueprintGetter=GetGravityDirection, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FVector GravityDirection = FVector(0.0, 0.0, -1.0);

protected:
	/**
	 * Max angle in degrees of a walkable surface. Any greater than this and it is too steep to be walkable.
	 */
	UPROPERTY(Category="Movement: Walking", EditAnywhere, BlueprintGetter=GetWalkableFloorAngle, BlueprintSetter=SetWalkableFloorAngle, meta=(ClampMin="0.0", ClampMax="90.0", UIMin = "0.0", UIMax = "90.0", ForceUnits="degrees"))
	float WalkableFloorAngle;

	/**
	 * Minimum Z value for floor normal. If less, not a walkable surface. Computed from WalkableFloorAngle.
	 */
	UPROPERTY(Category="Movement: Walking", VisibleAnywhere, BlueprintGetter=GetWalkableFloorZ, BlueprintSetter=SetWalkableFloorZ)
	float WalkableFloorZ;
	
	UPROPERTY()
	TWeakObjectPtr<AActor> CachedOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPendingForces PendingForces;
};

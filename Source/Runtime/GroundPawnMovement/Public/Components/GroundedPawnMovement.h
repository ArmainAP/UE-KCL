// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GroundedPawnMovement.generated.h"

/**
 * 
 */
UCLASS()
class GROUNDPAWNMOVEMENT_API UGroundedPawnMovement : public UFloatingPawnMovement
{
	GENERATED_BODY()

public:
	UGroundedPawnMovement();
	
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RotateTowardsMovement(float DeltaTime) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rotation")
	bool bRotateTowardMovement = true;
	
	/** Degrees-per-second interpolation speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rotation", meta=(EditCondition="bRotateTowardMovement"))
	float RotationSpeed = 720.f;

	/** Minimum horizontal speed before we start to rotate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rotation", meta=(EditCondition="bRotateTowardMovement"))
	float MinOrientSpeed = 10.f;

protected:
	UPROPERTY()
	TWeakObjectPtr<AActor> CachedOwner;
};

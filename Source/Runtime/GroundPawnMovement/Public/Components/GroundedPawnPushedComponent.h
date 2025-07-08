// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GroundedPawnAvoidanceSensing.h"
#include "Components/SphereComponent.h"
#include "Structs/GroundPawnPush.h"
#include "Structs/GroundPawnSensor.h"
#include "GroundedPawnPushedComponent.generated.h"

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class GROUNDPAWNMOVEMENT_API UGroundedPawnPushedComponent : public USphereComponent
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	
public:
	UFUNCTION(BlueprintPure)
	static UGroundedPawnPushedComponent* Find(AActor* Actor);

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void Push(FVector PushVector);
	
	UFUNCTION(BlueprintCallable)
	void PushSensedComponents() const;

	UFUNCTION(BlueprintCallable)
	void Reset();

	FVector ConsumePushForce(const UWorld* World);
protected:
	void AccumulatePushForceWhileMoving(const FVector& AccelerationFromForceAndMass, float Speed);
	void HandleHit(const UGroundedPawnAvoidanceSensing& Sensing, const FSensorCollisionData& Hit, const FVector& PusherDir) const;
	FVector ComputePushVectors(const UGroundedPawnAvoidanceSensing& Sensing, const FSensorCollisionData& Hit, const APawn& Pushed, const FVector& PusherDir) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGroundPawnPush PushSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<APawn> OwnerPawn;

	UPROPERTY(Transient)
	FVector PushAccumulatedForce = FVector::ZeroVector;
};
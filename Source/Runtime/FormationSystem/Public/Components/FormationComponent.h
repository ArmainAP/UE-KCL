// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FormationComponent.generated.h"

class AAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMovementStateChanged, UFormationComponent*, Unit);

UCLASS( BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent) )
class FORMATIONSYSTEM_API UFormationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFormationComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void StopMovement();

	UFUNCTION(BlueprintCallable)
	void SetupFormation(const FVector& InTargetLocation, const FRotator& InTargetRotation);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasReachedTargetLocation();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasReached();

protected:
	bool HandleRotation();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, BlueprintAssignable)
	FMovementStateChanged OnStopped;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, BlueprintAssignable)
	FMovementStateChanged OnMove;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, BlueprintAssignable)
	FMovementStateChanged OnReached;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bReached = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DestinationAcceptanceRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DestinationDistanceThreshold = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DestinationRotationRate = 5.0f;

	UPROPERTY()
	AAIController* OwnerController = nullptr;
	
	float CachedDeltaTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator TargetRotation;
};

// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GroundedPawnPusherComponent.generated.h"

class UGroundPathFollowingComponent;

/**
 * 
 */
UCLASS()
class GROUNDPAWNMOVEMENT_API UGroundedPawnPusherComponent : public USphereComponent
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	
public:
	UGroundedPawnPusherComponent();
	
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	static UGroundPathFollowingComponent* GetGroundPathFollowingComponent(AActor* Actor);
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0))
	float PushForceMultiplier = 1;

protected:
	UPROPERTY()
	TWeakObjectPtr<APawn> OwnerPawn;
	
	TSet<TWeakObjectPtr<UGroundPathFollowingComponent>, TWeakObjectPtrSetKeyFuncs<TWeakObjectPtr<UGroundPathFollowingComponent>>> OverlappedGroundPathFollowingComponents;
};

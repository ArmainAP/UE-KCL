// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GroundedPawnPushedComponent.h"
#include "Components/SphereTraceMultiComponent.h"
#include "GroundedPawnPusherComponent.generated.h"

class UGroundPathFollowingComponent;

/**
 * 
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class GROUNDPAWNMOVEMENT_API UGroundedPawnPusherComponent : public USphereTraceMultiComponent
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
	static UGroundedPawnPushedComponent* GetGroundedPawnPushedComponent(AActor* Actor);

	virtual void HandleActorTraceBegin_Implementation(const FHitResult& HitResult) override;
	virtual void HandleActorTraceEnd_Implementation(const FHitResult& HitResult) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0))
	float PushForceMultiplier = 1;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TWeakObjectPtr<APawn> OwnerPawn;
	
	TSet<TWeakObjectPtr<UGroundedPawnPushedComponent>, TWeakObjectPtrSetKeyFuncs<TWeakObjectPtr<UGroundedPawnPushedComponent>>> OverlappedGroundPushedComponents;
};

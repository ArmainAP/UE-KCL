// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FormationComponent.generated.h"

class UFormationContext;
class AAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFormationUnitEvent, UFormationComponent*, Unit);

UENUM()
enum class EMovementState : uint8
{
	Stopped,
	Moving,
	Reached
};

UCLASS( BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent), Category="Components|FormationSystem" )
class FORMATIONSYSTEM_API UFormationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFormationComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	void SetupTarget(const FTransform& InTransform);
	bool HasReached() const;
	FTransform GetTransform() const;
	void HandleFormationLeft(UFormationContext* OldFormationContext);
	void HandleFormationJoined(UFormationContext* NewFormationContext);
	float GetDistanceToDestination() const;

	UFUNCTION(BlueprintSetter)
	void SetMovementState(EMovementState InMovementState);
	
	UFUNCTION(BlueprintCallable)
	bool LeaveFormation();

	UFUNCTION(BlueprintPure)
	FVector GetTargetLocation() const;

	UFUNCTION(BlueprintPure)
	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure)
	UFormationContext* GetFormationContext() const { return FormationContext; }

	UFUNCTION(BlueprintGetter)
	bool CanRequestMovement() const { return bEnableMovementRequests; }

	UFUNCTION(BlueprintSetter)
	void EnableMovementRequest(const bool bEnabled)
	{
		bEnableMovementRequests = bEnabled;
	}

protected:
	void PerformDistanceToGroupCheck();
	float GetDistanceTo(const FVector& Location) const;

	void SetHasFallenBehind(bool NewHasFallenBehind);
	
public:
	UPROPERTY(BlueprintAssignable)
	FFormationUnitEvent OnMovementStopped;

	UPROPERTY(BlueprintAssignable)
	FFormationUnitEvent OnMovementRequest;

	UPROPERTY(BlueprintAssignable)
	FFormationUnitEvent OnMovementStarted;
	
	UPROPERTY(BlueprintAssignable)
	FFormationUnitEvent OnReached;

	UPROPERTY(BlueprintAssignable)
	FFormationUnitEvent OnFallBehind;

	UPROPERTY(BlueprintAssignable)
	FFormationUnitEvent OnCatchUp;

	UPROPERTY(BlueprintAssignable)
	FFormationUnitEvent OnLeftGroup;
	
	UPROPERTY(BlueprintAssignable)
	FFormationUnitEvent OnJoinedGroup;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintSetter=SetMovementState)
	EMovementState MovementState = EMovementState::Stopped;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CatchUpDistanceThreshold = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FallBehindDistanceThreshold = 5000.0f;
	
	UPROPERTY(VisibleAnywhere)
	FTransform TargetTransform;
	
	UPROPERTY()
	TObjectPtr<UFormationContext> FormationContext;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UFormationDataAsset> DefaultFormationDataAsset = nullptr;

	UPROPERTY()
	bool bHasFallenBehind = false;

	UPROPERTY()
	TObjectPtr<class UFormationSubsystem> CachedFormationSubsystem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintSetter=EnableMovementRequest, BlueprintGetter=CanRequestMovement)
	bool bEnableMovementRequests = true;
};

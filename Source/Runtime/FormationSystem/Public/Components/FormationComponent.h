// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FormationComponent.generated.h"

class AAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFormationUnitEvent, UFormationComponent*, Unit);

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
	void StartMovement();
	void StopMovement();
	void EndMovement();
	bool HasReached() const;
	FTransform GetTransform() const;
	void HandleFormationLeft(const FName OldFormation);
	void HandleFormationJoined(const FName NewFormation);
	float GetDistanceToDestination() const;

	UFUNCTION(BlueprintCallable)
	bool LeaveFormation();

	UFUNCTION(BlueprintPure)
	FName GetFormationID() const;
	
	UFUNCTION(BlueprintPure)
	FVector GetTargetLocation() const;

	UFUNCTION(BlueprintPure)
	FVector GetFormationLeadLocation() const;

	UFUNCTION(BlueprintPure)
	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure)
	AActor* GetFormationOwner() const;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bReached = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CatchUpDistanceThreshold = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FallBehindDistanceThreshold = 5000.0f;
	
	UPROPERTY(VisibleAnywhere)
	FTransform TargetTransform;
	
	UPROPERTY()
	FName FormationID = NAME_None;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UFormationDataAsset> DefaultFormationDataAsset = nullptr;

	UPROPERTY()
	bool bHasFallenBehind = false;

	UPROPERTY()
	TObjectPtr<class UFormationSubsystem> CachedFormationSubsystem = nullptr;
};

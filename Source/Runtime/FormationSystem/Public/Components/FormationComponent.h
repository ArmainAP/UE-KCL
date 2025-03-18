// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/FormationUnit.h"
#include "FormationComponent.generated.h"

class UFormationDataAsset;
class UFormationGroupInfo;
class AAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMovementStateChanged, UFormationComponent*, Unit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGroupChanged, UFormationGroupInfo*, GroupInfo);

UCLASS( BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent), Category="Components|FormationSystem" )
class FORMATIONSYSTEM_API UFormationComponent : public UActorComponent, public IFormationUnit
{
	GENERATED_BODY()

public:	
	UFormationComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	// Begin IFormationUnit
	virtual void SetupTarget_Implementation(const FVector& InTargetLocation, const FRotator& InTargetRotation) override;
	virtual void StopMovement_Implementation() override;
	virtual bool HasReached_Implementation() override;
	virtual FTransform GetTransform_Implementation() const override;
	virtual void HandleFormationLeft_Implementation(UFormationGroupInfo* OldFormation) override;
	virtual void HandleFormationJoined_Implementation(UFormationGroupInfo* NewFormation) override;
	virtual AActor* GetActor_Implementation() const override;
	virtual float GetDistanceToDestination_Implementation() const override;
	// End IFormationUnit

	UFUNCTION(BlueprintCallable)
	bool ChangeFormationGroup(UFormationGroupInfo* NewFormation);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UFormationGroupInfo* GetFormationGroupInfo();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetTargetLocation() const;

protected:
	void PerformDistanceToGroupCheck();
	bool HandleRotation();
	float GetDistanceTo(const FVector& Location) const;

	void SetHasFallenBehind(bool NewHasFallenBehind);
	
public:
	UPROPERTY(BlueprintAssignable)
	FMovementStateChanged OnStopped;

	UPROPERTY(BlueprintAssignable)
	FMovementStateChanged OnMove;
	
	UPROPERTY(BlueprintAssignable)
	FMovementStateChanged OnReached;

	UPROPERTY(BlueprintAssignable)
	FMovementStateChanged OnFallBehind;

	UPROPERTY(BlueprintAssignable)
	FMovementStateChanged OnCatchUp;

	UPROPERTY(BlueprintAssignable)
	FGroupChanged OnLeftGroup;
	
	UPROPERTY(BlueprintAssignable)
	FGroupChanged OnJoinedGroup;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bReached = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DestinationDistanceThreshold = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DestinationRotationRate = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CatchUpDistanceThreshold = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FallBehindDistanceThreshold = 5000.0f;
	
	UPROPERTY()
	AAIController* OwnerController = nullptr;

	UPROPERTY()
	float CachedDeltaTime = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, BlueprintGetter=GetTargetLocation)
	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRotator TargetRotation = FRotator::ZeroRotator;

	UPROPERTY()
	UFormationGroupInfo* GroupInfo = nullptr;

	UPROPERTY(EditAnywhere)
	UFormationDataAsset* DefaultFormationDataAsset = nullptr;

	UPROPERTY()
	bool bHasFallenBehind = false;
};

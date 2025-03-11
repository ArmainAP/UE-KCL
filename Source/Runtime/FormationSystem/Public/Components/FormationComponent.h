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
	// End IFormationUnit

	UFUNCTION(BlueprintCallable)
	bool ChangeFormationGroup(UFormationGroupInfo* NewFormation);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UFormationGroupInfo* GetFormationGroupInfo();

protected:
	bool HasReachedTargetLocation();
	bool HandleRotation() const;

public:
	UPROPERTY(BlueprintAssignable)
	FMovementStateChanged OnStopped;

	UPROPERTY(BlueprintAssignable)
	FMovementStateChanged OnMove;
	
	UPROPERTY(BlueprintAssignable)
	FMovementStateChanged OnReached;

	UPROPERTY(BlueprintAssignable)
	FGroupChanged OnLeftGroup;
	
	UPROPERTY(BlueprintAssignable)
	FGroupChanged OnJoinedGroup;
	
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

	UPROPERTY()
	float CachedDeltaTime = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRotator TargetRotation = FRotator::ZeroRotator;

	UPROPERTY()
	UFormationGroupInfo* GroupInfo = nullptr;

	UPROPERTY(EditAnywhere)
	UFormationDataAsset* DefaultFormationDataAsset = nullptr;
};

// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FormationComponent.generated.h"

class AAIController;

UENUM(Blueprintable, BlueprintType)
enum class EMovementState : uint8
{
	None,
	Moving,
	Reached
};

USTRUCT(Blueprintable, BlueprintType)
struct FFormationUnitData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GroupID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APawn* FormationLeader;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator TargetRotation;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMovementStateChanged, const EMovementState, MovementState);

UCLASS( BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent) )
class FORMATIONSYSTEM_API UFormationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFormationComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetMovementState(const EMovementState NewMovementState);

	UFUNCTION(BlueprintCallable)
	void SetupFormation(const FFormationUnitData& NewFormationUnitData);

	UFUNCTION(BlueprintPure)
	bool HasReachedTargetLocation(FVector& TargetLocation) const;

	UFUNCTION(BlueprintCallable)
	void OnFormationLeaderReached();

protected:
	bool HandleRotation();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, BlueprintAssignable)
	FMovementStateChanged OnMovementStateChanged;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EMovementState MovementState = EMovementState::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FFormationUnitData FormationUnitData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DestinationAcceptanceRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DestinationDistanceThreshold = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DestinationRotationRate = 5.0f;

	UPROPERTY()
	AAIController* OwnerController = nullptr;
	
	float CachedDeltaTime = 0.0f;
};

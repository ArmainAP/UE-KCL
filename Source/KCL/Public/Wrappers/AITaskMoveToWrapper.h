// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/AITask_MoveTo.h"
#include "AITaskMoveToWrapper.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct KCL_API FMoveTaskParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AcceptanceRadius = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EAIOptionFlag::Type> StopOnOverlap = EAIOptionFlag::Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EAIOptionFlag::Type> AcceptPartialPath = EAIOptionFlag::Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUsePathfinding = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLockAILogic = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseContinuousGoalTracking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EAIOptionFlag::Type> ProjectGoalOnNavigation = EAIOptionFlag::Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EAIOptionFlag::Type> RequireNavigableEndLocation = EAIOptionFlag::Default;
};

UCLASS()
class KCL_API UAITask_MoveTo_Wrapper : public UAITask_MoveTo
{
	GENERATED_BODY()

public:
	static UAITask_MoveTo_Wrapper* AIMoveTo(AAIController* Controller, const FVector& InGoalLocation, const AActor* InGoalActor, const FMoveTaskParameters& MoveTaskParameters);
	
	FGenericGameplayTaskDelegate& GetRequestFailed();
	FMoveTaskCompletedSignature& GetMoveFinished();
};
// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Wrappers/AITaskMoveToWrapper.h"
#include "AIController.h"

UAITask_MoveTo_Wrapper* UAITask_MoveTo_Wrapper::AIMoveTo(AAIController* Controller, const FVector& InGoalLocation, const AActor* InGoalActor, const FMoveTaskParameters& MoveTaskParameters)
{
	UAITask_MoveTo_Wrapper* MyTask = Controller ? NewAITask<UAITask_MoveTo_Wrapper>(*Controller, EAITaskPriority::High) : nullptr;
	if (MyTask)
	{
		FAIMoveRequest MoveReq;
		if (InGoalActor)
		{
			MoveReq.SetGoalActor(InGoalActor);
		}
		else
		{
			MoveReq.SetGoalLocation(InGoalLocation);
		}

		MoveReq.SetAcceptanceRadius(MoveTaskParameters.AcceptanceRadius);
		MoveReq.SetReachTestIncludesAgentRadius(FAISystem::PickAIOption(MoveTaskParameters.StopOnOverlap, MoveReq.IsReachTestIncludingAgentRadius()));
		MoveReq.SetAllowPartialPath(FAISystem::PickAIOption(MoveTaskParameters.AcceptPartialPath, MoveReq.IsUsingPartialPaths()));
		MoveReq.SetUsePathfinding(MoveTaskParameters.bUsePathfinding);
		MoveReq.SetProjectGoalLocation(FAISystem::PickAIOption(MoveTaskParameters.ProjectGoalOnNavigation, MoveReq.IsProjectingGoal()));
		MoveReq.SetRequireNavigableEndLocation(FAISystem::PickAIOption(MoveTaskParameters.RequireNavigableEndLocation, MoveReq.IsNavigableEndLocationRequired()));
		if (Controller)
		{
			MoveReq.SetNavigationFilter(Controller->GetDefaultNavigationFilterClass());
		}

		MyTask->SetUp(Controller, MoveReq);
		MyTask->SetContinuousGoalTracking(MoveTaskParameters.bUseContinuousGoalTracking);

		if (MoveTaskParameters.bLockAILogic)
		{
			MyTask->RequestAILogicLocking();
		}
	}

	return MyTask;
}

UGameplayTask::FGenericGameplayTaskDelegate& UAITask_MoveTo_Wrapper::GetRequestFailed()
{
	return OnRequestFailed;
}

FMoveTaskCompletedSignature& UAITask_MoveTo_Wrapper::GetMoveFinished()
{
	return OnMoveFinished;
}

void UAITask_MoveTo_Wrapper::InvalidateMoveTask()
{
	OnContextInvalidated.Broadcast(this);
	OnMoveFinished.RemoveDynamic(this, &UAITask_MoveTo_Wrapper::HandleMoveFinished);
	OnRequestFailed.RemoveDynamic(this, &UAITask_MoveTo_Wrapper::HandleRequestFailed);
	EndTask();
}

void UAITask_MoveTo_Wrapper::Activate()
{
	Super::Activate();
	
	OnMoveFinished.AddUniqueDynamic(this, &UAITask_MoveTo_Wrapper::HandleMoveFinished);
	OnRequestFailed.AddUniqueDynamic(this, &UAITask_MoveTo_Wrapper::HandleRequestFailed);
}

void UAITask_MoveTo_Wrapper::HandleRequestFailed()
{
	OnContextFailed.Broadcast(this);
}

void UAITask_MoveTo_Wrapper::HandleMoveFinished(TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController)
{
	if (Result != EPathFollowingResult::Success)
	{
		OnContextFailed.Broadcast(this);
	}
	else
	{
		OnContextFinished.Broadcast(this);
	}

	InvalidateMoveTask();
}

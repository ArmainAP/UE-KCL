// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/TaskMoveToComponent.h"

#include "KiraHelperLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

static TAutoConsoleVariable CVar_Debug(TEXT("KCL.Debug.UTaskMoveToComponent"), false, TEXT("Global debug toggle"), ECVF_Cheat);

UTaskMoveToComponent::UTaskMoveToComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTaskMoveToComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsValid(MoveToContext) && CVar_Debug.GetValueOnGameThread())
	{
		const UWorld* World = GetWorld();
		const FVector Location = MoveToContext->GetMoveRequestRef().GetDestination();
		UKismetSystemLibrary::DrawDebugSphere(World, Location, 100, 12, FColor::Blue, DeltaTime);
		UKismetSystemLibrary::DrawDebugString(World, Location, GetOwner()->GetName(), nullptr, FColor::White, DeltaTime);
	}
}

bool UTaskMoveToComponent::GetDesiredLocation(FVector& OutLocation) const
{
	const bool bIsValid = IsValid(MoveToContext);
	if (bIsValid)
	{
		OutLocation = MoveToContext->GetMoveRequestRef().GetDestination();
	}
	return bIsValid;
}

void UTaskMoveToComponent::SetupTask(const FVector& Location, const AActor* Actor)
{
	if (IsValid(MoveToContext))
	{
		MoveToContext->InvalidateMoveTask();
	}

	MoveToContext = UAITask_MoveTo_Wrapper::AIMoveTo(UKiraHelperLibrary::GetAIController(GetOwner()), Location, Actor, MoveTaskParameters);
	if (MoveToContext)
	{
		MoveToContext->GetRequestFailed().AddUniqueDynamic(this, &UTaskMoveToComponent::HandleRequestFailed);
		MoveToContext->GetMoveFinished().AddUniqueDynamic(this, &UTaskMoveToComponent::HandleMoveFinished);
		MoveToContext->ReadyForActivation();
		OnMoveStarted.Broadcast(this, MoveToContext);
	}
}

UAITask_MoveTo_Wrapper* UTaskMoveToComponent::MoveToActor_Implementation(const AActor* Actor, bool bForce)
{
	UE_LOG(LogTemp, Display, TEXT("%s >> %s"), *GetOwner()->GetName(), StringCast<TCHAR>(__FUNCTION__).Get());
	if (!Actor) { return MoveToContext; }
	if (!bForce && IsValid(MoveToContext))
	{
		FAIMoveRequest& Request = MoveToContext->GetMoveRequestRef();
		if (Request.IsMoveToActorRequest())
		{
			Request.SetGoalActor(Actor);
			return MoveToContext;
		}
	}
	
	SetupTask(Actor->GetActorLocation(), Actor);
	return MoveToContext;
}

UAITask_MoveTo_Wrapper* UTaskMoveToComponent::MoveToLocation_Implementation(const FVector& Location, bool bForce)
{
	UE_LOG(LogTemp, Display, TEXT("%s >> %s >> %s"), *GetOwner()->GetName(), StringCast<TCHAR>(__FUNCTION__).Get(), *Location.ToCompactString());
	const bool bHasContext = IsValid(MoveToContext);
	if (!bForce && bHasContext)
	{
		FAIMoveRequest& Request = MoveToContext->GetMoveRequestRef();
		if (!Request.IsMoveToActorRequest())
		{
			Request.SetGoalLocation(Location);
			return MoveToContext;
		}
	}

	SetupTask(Location);
	return MoveToContext;
}
void UTaskMoveToComponent::StopMovement_Implementation()
{
	UE_LOG(LogTemp, Display, TEXT("%s >> %s"), *GetOwner()->GetName(), StringCast<TCHAR>(__FUNCTION__).Get());
	if (IsValid(MoveToContext))
	{
		OnMoveStopped.Broadcast(this, MoveToContext);
		MoveToContext->InvalidateMoveTask();
	}
}

void UTaskMoveToComponent::HandleRequestFailed()
{
	MoveToContext->OnContextFailed.Broadcast(MoveToContext);
}

void UTaskMoveToComponent::HandleMoveFinished(TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController)
{
	if (Result != EPathFollowingResult::Success)
	{
		MoveToContext->OnContextFailed.Broadcast(MoveToContext);
	}
	else
	{
		MoveToContext->OnContextFinished.Broadcast(MoveToContext);
	}
}

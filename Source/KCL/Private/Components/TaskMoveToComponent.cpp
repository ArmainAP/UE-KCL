// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/TaskMoveToComponent.h"

#include "KiraHelperLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

static TAutoConsoleVariable CVar_Debug(TEXT("KCL.Debug.UTaskMoveToComponent"), false, TEXT("Global debug toggle"), ECVF_Cheat);

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

UAITask_MoveTo_Wrapper* UTaskMoveToComponent::MoveToActor_Implementation(const AActor* Actor, bool bForce)
{
	if (!Actor) { return MoveToContext; }
	const bool bHasContext = IsValid(MoveToContext);
	if (!bForce && bHasContext)
	{
		FAIMoveRequest& Request = MoveToContext->GetMoveRequestRef();
		if (Request.IsMoveToActorRequest())
		{
			Request.SetGoalActor(Actor);
			return MoveToContext;
		}
	}
	
	if (bHasContext)
	{
		MoveToContext->InvalidateMoveTask();
	}

	MoveToContext = UAITask_MoveTo_Wrapper::AIMoveTo(UKiraHelperLibrary::GetAIController(GetOwner()),
		Actor->GetActorLocation(), Actor, MoveTaskParameters);
	OnMoveStarted.Broadcast(this, MoveToContext);
	return MoveToContext;
}

UAITask_MoveTo_Wrapper* UTaskMoveToComponent::MoveToLocation_Implementation(const FVector& Location, bool bForce)
{
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

	if (bHasContext)
	{
		MoveToContext->InvalidateMoveTask();
	}

	MoveToContext = UAITask_MoveTo_Wrapper::AIMoveTo(UKiraHelperLibrary::GetAIController(GetOwner()),
		Location, nullptr, MoveTaskParameters);
	OnMoveStarted.Broadcast(this, MoveToContext);
	return MoveToContext;
}
void UTaskMoveToComponent::StopMovement_Implementation()
{
	UE_LOG(LogTemp, Display, TEXT("%s >> %s"), *GetOwner()->GetName(), StringCast<TCHAR>(__FUNCTION__).Get());
	OnMoveStopped.Broadcast(this, MoveToContext);
}

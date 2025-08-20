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

void UTaskMoveToComponent::InvalidateMoveTask()
{
	if (MoveToContext)
	{
		OnContextInvalidated.Broadcast(this, MoveToContext->GetUniqueID());
		MoveToContext->EndTask();
		MoveToContext = nullptr;
	}
}
bool UTaskMoveToComponent::GetDesiredLocation(FVector& OutLocation) const
{
	const bool bIsValid = MoveToContext != nullptr;
	if (MoveToContext)
	{
		OutLocation = MoveToContext->GetMoveRequestRef().GetDestination();
	}
	return bIsValid;
}

void UTaskMoveToComponent::SetupTask(const FVector& Location, const AActor* Actor)
{
	if (MoveToContext)
	{
		InvalidateMoveTask();
	}

	MoveToContext = UAITask_MoveTo_Wrapper::AIMoveTo(UKiraHelperLibrary::GetAIController(GetOwner()), Location, Actor, MoveTaskParameters);
	MoveToContext->GetRequestFailed().AddUniqueDynamic(this, &UTaskMoveToComponent::HandleRequestFailed);
	MoveToContext->GetMoveFinished().AddUniqueDynamic(this, &UTaskMoveToComponent::HandleMoveFinished);
	MoveToContext->ReadyForActivation();
	OnMoveStarted.Broadcast(this, MoveToContext->GetUniqueID());
}

int UTaskMoveToComponent::MoveToActor_Implementation(const AActor* Actor, bool bForce)
{
	UE_LOG(LogTemp, Display, TEXT("%s >> %s"), *GetOwner()->GetName(), StringCast<TCHAR>(__FUNCTION__).Get());
	if (!Actor) { return INDEX_NONE; }
	if (!bForce && IsValid(MoveToContext))
	{
		FAIMoveRequest& Request = MoveToContext->GetMoveRequestRef();
		if (Request.IsMoveToActorRequest())
		{
			Request.SetGoalActor(Actor);
			return MoveToContext->GetUniqueID();
		}
	}
	
	SetupTask(Actor->GetActorLocation(), Actor);
	return MoveToContext ? MoveToContext->GetUniqueID() : INDEX_NONE;
}

int UTaskMoveToComponent::MoveToLocation_Implementation(const FVector& Location, bool bForce)
{
	if (FVector DesiredLocation; !bForce && GetDesiredLocation(DesiredLocation))
	{
		if (FVector::Dist(DesiredLocation, Location) < MoveTaskParameters.AcceptanceRadius)
		{
			return MoveToContext->GetUniqueID();
		}
	}
	
	UE_LOG(LogTemp, Display, TEXT("%s >> %s >> %s"), *GetOwner()->GetName(), StringCast<TCHAR>(__FUNCTION__).Get(), *Location.ToCompactString());
	const bool bHasContext = IsValid(MoveToContext);
	if (!bForce && bHasContext)
	{
		FAIMoveRequest& Request = MoveToContext->GetMoveRequestRef();
		if (!Request.IsMoveToActorRequest())
		{
			Request.SetGoalLocation(Location);
			return MoveToContext->GetUniqueID();
		}
	}

	SetupTask(Location);
	return MoveToContext ? MoveToContext->GetUniqueID() : INDEX_NONE;
}
void UTaskMoveToComponent::StopMovement_Implementation()
{
	if (MoveToContext)
	{
		UE_LOG(LogTemp, Display, TEXT("%s >> %s"), *GetOwner()->GetName(), StringCast<TCHAR>(__FUNCTION__).Get());
		OnMoveStopped.Broadcast(this, MoveToContext->GetUniqueID());
		InvalidateMoveTask();
	}
}

void UTaskMoveToComponent::HandleRequestFailed()
{
	const int MoveID = MoveToContext ? MoveToContext->GetUniqueID() : INDEX_NONE;
	OnContextFailed.Broadcast(this, MoveID);
}

void UTaskMoveToComponent::HandleMoveFinished(TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController)
{
	const int MoveID = MoveToContext ? MoveToContext->GetUniqueID() : INDEX_NONE;
	if (Result != EPathFollowingResult::Success)
	{
		OnContextFailed.Broadcast(this, MoveID);
	}
	else
	{
		OnContextFinished.Broadcast(this, MoveID);
	}
}

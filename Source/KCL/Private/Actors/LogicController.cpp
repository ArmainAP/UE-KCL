// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Actors/LogicController.h"

ALogicController::ALogicController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	bStartAILogicOnPossess = true;
	bStopAILogicOnUnposses = true;
}

void ALogicController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bShouldRunLogic)
	{
		RunLogic(DeltaSeconds);
	}
}

void ALogicController::StartAILogic_Implementation(APawn* InNewPawn)
{
	bShouldRunLogic = true;
	SetActorTickEnabled(true);
}

void ALogicController::StopAILogic_Implementation()
{
	bShouldRunLogic = false;
	SetActorTickEnabled(false);
}

void ALogicController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!bStartAILogicOnPossess)
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimerForNextTick([this, InPawn]
	{
		StartAILogic(InPawn);
	});
}

void ALogicController::OnUnPossess()
{
	if (bStopAILogicOnUnposses)
	{
		StopAILogic();
	}

	Super::OnUnPossess();
}
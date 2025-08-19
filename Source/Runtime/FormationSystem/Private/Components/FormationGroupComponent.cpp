// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/FormationGroupComponent.h"

#include "Data/FormationDataAssets/FormationDataAsset.h"
#include "Debug/DebugDrawService.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Objects/FormationContext.h"
#include "Subsystems/FormationSubsystem.h"

static TAutoConsoleVariable<bool> CVar_Debug(TEXT("KCL.Debug.UFormationGroupComponent"), false, TEXT("Toggles component debug"));

void UFormationGroupComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (ID.IsEmpty())
	{
		ID = FGuid::NewGuid().ToString();
	}

	Context = UFormationSubsystem::Get(GetWorld())->CreateGroup(ID, DefaultFormationDataAsset, GetOwner());
	check(Context);
	Context->OnUnitJoined.AddUniqueDynamic(this, &UFormationGroupComponent::OnUnitJoined);

	DebugHandle = UDebugDrawService::Register(TEXT("Game"), FDebugDrawDelegate::CreateUObject(this, &UFormationGroupComponent::OnDebugDraw));
}

void UFormationGroupComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UDebugDrawService::Unregister(DebugHandle);

	if (bAutoDestroyFormation && Context)
	{
		UFormationSubsystem::Get(GetWorld())->DestroyGroup(ID);
	}
	
	Super::EndPlay(EndPlayReason);
}

FTransform UFormationGroupComponent::GetUnitWorldTransform(const int Index) const
{
	const FTransform Transform = Context->GetTranformForIndex(Index);
	return Transform * GetComponentTransform();
}

FTransform UFormationGroupComponent::GetNextUnitWorldTransform() const
{
	return GetUnitWorldTransform(Context->GetUnitsCount());
}

void UFormationGroupComponent::RequestMove() const
{
	if (Context)
	{
		Context->RequestMove(GetComponentTransform());
	}
}

void UFormationGroupComponent::OnUnitJoined(UFormationContext* InContext, UFormationComponent* InUnit)
{
	InUnit->SetupTarget(GetUnitWorldTransform(InContext->GetUnitsCount() - 1));
}

void UFormationGroupComponent::OnDebugDraw(UCanvas* Canvas, APlayerController* PlayerController) const
{
	if (!(CVar_Debug.GetValueOnGameThread() || FormationGroupComponentDebug.bDebug) || !Context)
	{
		return;
	}

	const int UnitCount = Context->GetUnitsCount();
	if (UnitCount < 1)
	{
		return;
	}
	
	const UFormationDataAsset* FormationDataAsset = Context->GetFormationDataAsset();
	const int FormationCount = FormationDataAsset->GetCappedUnitCount(UnitCount);

	TArray<FTransform> OutTransforms;
	FormationDataAsset->GetOffsetTransforms(FormationCount, OutTransforms);

	const FTransform ComponentTransform = GetComponentTransform();
	const UWorld* World = GetWorld();
	for (FTransform Transform : OutTransforms)
	{
		Transform = Transform * ComponentTransform;
		UKismetSystemLibrary::DrawDebugSphere(World, Transform.GetLocation(), FormationGroupComponentDebug.Radius, FormationGroupComponentDebug.Segments, FormationGroupComponentDebug.LineColor, 0, FormationGroupComponentDebug.Thickness);
	}
}

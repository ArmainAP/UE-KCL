// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Components/FormationGroupComponent.h"

#include "Data/FormationDataAssets/FormationDataAsset.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Objects/FormationContext.h"
#include "Subsystems/FormationSubsystem.h"

static TAutoConsoleVariable<bool> CVar_Debug(TEXT("KCL.Debug.UFormationGroupComponent"), false, TEXT("Toggles component debug"));

UFormationGroupComponent::UFormationGroupComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1;
}

void UFormationGroupComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (ID.IsEmpty())
	{
		ID = FGuid::NewGuid().ToString();
	}

	Context = UFormationSubsystem::Get(GetWorld())->CreateGroup(ID, DefaultFormationDataAsset, GetOwner());
	check(Context);
}

void UFormationGroupComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (FormationGroupComponentDebug.bDebug || CVar_Debug.GetValueOnGameThread())
	{
		DrawDebug(DeltaTime);
	}
	
	if (Context)
	{
		const FVector& FormationDirection = bUseWorldDirection ? Direction : GetComponentRotation().RotateVector(Direction);
		Context->RequestMove(GetComponentLocation(), FormationDirection);
	}
}

void UFormationGroupComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bAutoDestroyFormation && Context)
	{
		UFormationSubsystem::Get(GetWorld())->DestroyGroup(ID);
	}
	
	Super::EndPlay(EndPlayReason);
}

FTransform UFormationGroupComponent::GetUnitWorldTransform(const int Index) const
{
	TArray<FTransform> OutTransforms;
	const FVector& FormationDirection = bUseWorldDirection ? Direction : GetComponentRotation().RotateVector(Direction);
	Context->GetFormationDataAsset()->GetWorldTransforms(Index + 1, GetComponentLocation(), FormationDirection, OutTransforms);
	return OutTransforms[Index];
}

FTransform UFormationGroupComponent::GetNextUnitWorldTransform() const
{
	return GetUnitWorldTransform(Context->GetUnitsCount());
}

void UFormationGroupComponent::DrawDebug(const float DeltaTime) const
{
	TArray<FTransform> OutTransforms;
	const FVector& FormationDirection = bUseWorldDirection ? Direction : GetComponentRotation().RotateVector(Direction);

	const UFormationDataAsset* FormationDataAsset = Context->GetFormationDataAsset();
	const int FormationCount = FormationDataAsset->GetFormationLimit() != INDEX_NONE ? FormationDataAsset->GetFormationLimit() : Context->GetUnitsCount(); 
	FormationDataAsset->GetWorldTransforms(FormationCount, GetComponentLocation(), FormationDirection, OutTransforms);

	const UWorld* World = GetWorld();
	for (const FTransform& Transform : OutTransforms)
	{
		UKismetSystemLibrary::DrawDebugSphere(World, Transform.GetLocation(), FormationGroupComponentDebug.Radius, FormationGroupComponentDebug.Segments, FormationGroupComponentDebug.LineColor, DeltaTime, FormationGroupComponentDebug.Thickness);
		UKismetSystemLibrary::DrawDebugArrow(World, Transform.GetLocation(), Transform.GetLocation() + FormationDirection, FormationGroupComponentDebug.Radius, FormationGroupComponentDebug.LineColor, DeltaTime, FormationGroupComponentDebug.Thickness);
	}
}

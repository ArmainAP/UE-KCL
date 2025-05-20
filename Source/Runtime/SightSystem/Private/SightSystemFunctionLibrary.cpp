// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "SightSystemFunctionLibrary.h"
#include "Components/SightedComponent.h"
#include "Components/SighterComponent.h"
#include "Data/SightQueryContext.h"
#include "Kismet/KismetSystemLibrary.h"

FGenericTeamId USightSystemFunctionLibrary::GetComponentTeamId(const UActorComponent* Component)
{
	if (!Component)
	{
		return FGenericTeamId::NoTeam;
	}
	
	const IGenericTeamAgentInterface* TeamAgentInterface = Cast<IGenericTeamAgentInterface>(Component->GetOwner());
	if (!TeamAgentInterface)
	{
		return FGenericTeamId::NoTeam;
	}

	return TeamAgentInterface->GetGenericTeamId();
}

bool USightSystemFunctionLibrary::ShouldSight(const USighterComponent* SighterComponent, const USightedComponent* SightedComponent)
{
	const FGenericTeamId SighterTeamId = GetComponentTeamId(SighterComponent);
	const FGenericTeamId SightedTeamId = GetComponentTeamId(SightedComponent);
	if (SighterTeamId == FGenericTeamId::NoTeam || SightedTeamId == FGenericTeamId::NoTeam)
	{
		return false;
	}

	const AActor* SighterOwner = SighterComponent->GetOwner();
	const AActor* SightedOwner = SightedComponent->GetOwner();
	if (!SighterOwner || !SightedOwner || SighterOwner == SightedOwner)
	{
		return false;
	}
	
	if (!SighterComponent->SightDataAsset)
	{
		return false;
	}
	
	return FAISenseAffiliationFilter::ShouldSenseTeam(SighterTeamId, SightedTeamId, SighterComponent->SightDataAsset->DetectionByAffiliation.GetAsFlags());
}

bool USightSystemFunctionLibrary::IsVisibleInsideCone(FSightQueryContext& Query, const float MaxRadius, const float FieldOfView)
{
	if (!Query.Sighter.IsValid() || !Query.Sighted.IsValid())
	{
		return false;
	}
	
	UWorld* const World = Query.Sighter->GetWorld();
	if (!World)
	{
		return false;
	}

	// ——— Gather invariant data up front ———
	const FVector SourceLocation = Query.Sighter->GetComponentLocation();
	const FVector TargetLocation = Query.Sighted->GetComponentLocation();
	const FVector ToTarget = TargetLocation - SourceLocation;

	// 1) Range gate
	if (ToTarget.SizeSquared() > FMath::Square(MaxRadius))
	{
		return false;
	}

	// 2) Cone (use cosine to avoid acos + deg→rad round-trip)
	const FVector Forward = Query.Sighter->GetForwardVector();
	const float CosHalfFOV = FMath::Cos(FMath::DegreesToRadians(FieldOfView * 0.5f));
	const float CosAngle = FVector::DotProduct(ToTarget.GetSafeNormal(), Forward);
	if (CosAngle < CosHalfFOV)
	{
		return false;
	}

	// 3) Line-of-sight
	FHitResult Hit;
	UKismetSystemLibrary::LineTraceSingle(World, SourceLocation, TargetLocation, UEngineTypes::ConvertToTraceType(Query.Sighter->SightDataAsset->SightCollisionChannel), Query.Sighter->SightDataAsset->bTraceComplex, { Query.Sighter->GetOwner() }, Query.Sighter->SightDataAsset->DrawDebugTrace, Hit, true);
	if (Hit.GetActor() != Query.Sighted->GetOwner())
	{
		return false;   // an obstruction exists
	}

	// 4) Success: mark and report
	Query.bIsVisible = true;
	return true;
}

void USightSystemFunctionLibrary::EvaluateQueryVisibility(FSightQueryContext& Query)
{
	Query.bCurrenCheckSucceeded = false;
	Query.bIsVisible = false;
	
	if (!Query.Sighter.IsValid() || !Query.Sighted.IsValid())
	{
		return;
	}

	// Already perceived – use “lose sight” cone.
	if (Query.bIsPerceived)
	{
		const USightSystemDataAsset* Settings = Query.Sighter->SightDataAsset;
		Query.bCurrenCheckSucceeded = IsVisibleInsideCone(Query, Settings->LoseSightDistance, Settings->LoseSightFieldOfView);
		return;
	}

	// Not perceived – try the vision tiers.
	float HighestGainRate = TNumericLimits<float>::Lowest();

	float TestRadius = Query.Sighter->SightDataAsset->Radius + Query.bPreviousCheckSucceeded;
	if (!IsVisibleInsideCone(Query, TestRadius, Query.Sighter->SightDataAsset->FieldOfView))
	{
		return;
	}

	if (Query.bIsVisible)
	{
		HighestGainRate = FMath::Max(HighestGainRate, Query.Sighted->GainRate);
	}

	Query.GainRate = Query.Sighter->SightDataAsset->GainRateMultiplier * HighestGainRate;
	Query.bCurrenCheckSucceeded = true;
}

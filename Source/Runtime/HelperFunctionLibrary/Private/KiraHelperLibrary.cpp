// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "KiraHelperLibrary.h"
#include "AIController.h"
#include "NavigationSystem.h"

AAIController* UKiraHelperLibrary::GetAIController(AActor* Actor)
{
	if (const APawn* AsPawn = Cast<APawn>(Actor))
	{
		return Cast<AAIController>(AsPawn->GetController());
	}
	return Cast<AAIController>(Actor);
}

APawn* UKiraHelperLibrary::GetPawn(AActor* Actor)
{
	if (const AAIController* AsAIC = Cast<AAIController>(Actor))
	{
		return AsAIC->GetPawn();
	}
	return Cast<APawn>(Actor);
}

bool UKiraHelperLibrary::GetNavigablePathLenght(UWorld* WorldContextObject, const FVector& Start, const FVector& End, float& OutLength)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(WorldContextObject);
	if (!NavSys)
	{
		return false;
	}

	const ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
	if (!NavData)
	{
		return false;
	}

	FNavAgentProperties AgentProps;
	FPathFindingQuery Query;
	FPathFindingResult Result;

	Query = FPathFindingQuery(nullptr, *NavData, Start, End);
	Result = NavSys->FindPathSync(AgentProps, Query);
	if (!Result.IsSuccessful())
	{
		return false;
	}

	OutLength = Result.Path->GetLength();
	return true;
}

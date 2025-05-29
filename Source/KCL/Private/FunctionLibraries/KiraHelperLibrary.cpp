// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "FunctionLibraries/KiraHelperLibrary.h"

#include "AIController.h"

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

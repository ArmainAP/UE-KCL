// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Actors/WorldActorSpawner.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"

EDataValidationResult AWorldActorSpawner::IsDataValid(class FDataValidationContext& Context) const
{
	if (Super::IsDataValid(Context) == EDataValidationResult::Invalid)
	{
		return EDataValidationResult::Invalid;
	}

	if (!SpawnHandler)
	{
		const FString Error = FString::Printf(TEXT("%s is invalid"), GET_MEMBER_NAME_STRING_CHECKED(AWorldActorSpawner, SpawnHandler));
		Context.AddError(FText::FromString(Error));
		return EDataValidationResult::Invalid;
	}
	
	return SpawnHandler->IsDataValid(Context);
}
#endif // WITH_EDITOR

AWorldActorSpawner::AWorldActorSpawner(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWorldActorSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnHandler)
	{
		SpawnHandler->Activate();
	}
}

void AWorldActorSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (SpawnHandler)
	{
		SpawnHandler->Cancel();
	}
	
	Super::EndPlay(EndPlayReason);
}

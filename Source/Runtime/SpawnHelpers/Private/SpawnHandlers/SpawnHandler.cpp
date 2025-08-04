// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "SpawnHandlers/SpawnHandler.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"

EDataValidationResult USpawnHandler::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!ActorToSpawn)
	{
		const FString Error = FString::Printf(TEXT("%s is invalid"), GET_MEMBER_NAME_STRING_CHECKED(USpawnHandler, ActorToSpawn));
		Context.AddError(FText::FromString(Error));
		Result = EDataValidationResult::Invalid;
	}
	
	return Result;
}
#endif // WITH_EDITOR

void USpawnHandler::Activate()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		Cancel();
		return;
	}

	// Set up the spawn timer
	World->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &USpawnHandler::RequestSpawn), SpawnInterval, true, FirstSpawnDelay);
}

void USpawnHandler::Cancel()
{
	ClearTimers();
	Super::Cancel();
}

void USpawnHandler::RequestSpawn_Implementation()
{
	BeginSpawnActor();
}

FTransform USpawnHandler::GetSpawnActorTransform_Implementation() const
{
	FTransform SpawnTransform;
	if (const AActor* OuterActor = Cast<AActor>(GetOuter()))
	{
		SpawnTransform = OuterActor->GetActorTransform();
	}
	return SpawnTransform;
}

void USpawnHandler::BeginSpawnActor_Implementation()
{
	SpawnActor(GetSpawnActorTransform());
}

void USpawnHandler::SpawnActor_Implementation(const FTransform& Transform)
{
	check(ActorToSpawn)
	UWorld* World = GetWorld();
	check(World);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = SpawnCollisionHandlingOverride;
	PostSpawnActor(World->SpawnActor<AActor>(ActorToSpawn, Transform, SpawnParameters));
}

void USpawnHandler::ClearTimers()
{
	if (const UWorld* World = GetWorld(); IsValid(World))
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.ClearTimer(TimerHandle);
	}
}

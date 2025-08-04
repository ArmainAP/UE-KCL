// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Actors/ContinuousActorSpawner.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"

EDataValidationResult AContinuousActorSpawner::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!ActorToSpawn)
	{
		const FString Error = FString::Printf(TEXT("%s is invalid"), GET_MEMBER_NAME_STRING_CHECKED(AContinuousActorSpawner, ActorToSpawn));
		Context.AddError(FText::FromString(Error));
		Result = EDataValidationResult::Invalid;
	}
	
	return Result;
}
#endif // WITH_EDITOR

AContinuousActorSpawner::AContinuousActorSpawner(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

FTransform AContinuousActorSpawner::GetNextSpawnTransform_Implementation() const
{
	return GetActorTransform();
}

void AContinuousActorSpawner::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickInterval(SpawnInterval);
	if (SpawnOnStart)
	{
		Spawn();
	}

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &AContinuousActorSpawner::Spawn), SpawnInterval, true);
}

void AContinuousActorSpawner::Spawn_Implementation()
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = SpawnCollisionHandlingOverride;
	GetWorld()->SpawnActor<AActor>(ActorToSpawn, GetNextSpawnTransform(), SpawnParameters);
}

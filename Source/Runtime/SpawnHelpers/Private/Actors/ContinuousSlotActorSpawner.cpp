// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Actors/ContinuousSlotActorSpawner.h"

#include "Objects/FormationContext.h"
#include "Runtime/FormationSystem/Public/Components/FormationGroupComponent.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"

EDataValidationResult AContinuousSlotActorSpawner::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	const AActor* ActorCDO = Cast<AActor>(ActorToSpawn->ClassDefaultObject);
	if (!ActorCDO->GetComponentByClass<UFormationComponent>())
	{
		const FString Error = FString::Printf(TEXT("%s does not contain a FormationComponent"), GET_MEMBER_NAME_STRING_CHECKED(AContinuousSlotActorSpawner, ActorToSpawn));
		Context.AddError(FText::FromString(Error));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif // WITH_EDITOR

AContinuousSlotActorSpawner::AContinuousSlotActorSpawner(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	FormationGroupComponent = CreateDefaultSubobject<UFormationGroupComponent>(GET_MEMBER_NAME_CHECKED(AContinuousSlotActorSpawner, FormationGroupComponent));
	FormationGroupComponent->SetupAttachment(RootComponent);
}

FTransform AContinuousSlotActorSpawner::GetNextSpawnTransform_Implementation() const
{
	return FormationGroupComponent->GetUnitWorldTransform(FormationGroupComponent->GetContext()->GetUnitsCount());
}

void AContinuousSlotActorSpawner::Spawn_Implementation()
{
	const UFormationContext* FormationContext = FormationGroupComponent->GetContext();
	if (FormationContext->IsFull())
	{
		return;
	}
	
	Super::Spawn_Implementation();
}
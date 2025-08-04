// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "SpawnHandlers/FormationSlotSpawnHandler.h"

#include "Components/FormationGroupComponent.h"
#include "Objects/FormationContext.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"

EDataValidationResult UFormationSlotSpawnHandler::IsDataValid(class FDataValidationContext& Context) const
{
	if (Super::IsDataValid(Context) == EDataValidationResult::Invalid)
	{
		return EDataValidationResult::Invalid;
	}

	const AActor* OuterActor = Cast<AActor>(GetOuter());
	if (OuterActor && !OuterActor->GetComponentByClass<UFormationGroupComponent>())
	{
		const FString Error = FString::Printf(TEXT("%s requires an outer with %s"), *GetName(), *UFormationGroupComponent::StaticClass()->GetName());
		Context.AddError(FText::FromString(Error));
		return EDataValidationResult::Invalid;
	}

	return EDataValidationResult::Valid;
}

void UFormationSlotSpawnHandler::Activate()
{
	const AActor* OuterActor = Cast<AActor>(GetOuter());
	check(OuterActor);
	
	FormationGroupComponent = OuterActor->GetComponentByClass<UFormationGroupComponent>();
	check(FormationGroupComponent);
	
	Super::Activate();
}
#endif // WITH_EDITOR

FTransform UFormationSlotSpawnHandler::GetSpawnActorTransform_Implementation() const
{
	check(FormationGroupComponent);
	return FormationGroupComponent->GetNextUnitWorldTransform();
}

void UFormationSlotSpawnHandler::BeginSpawnActor_Implementation()
{
	const UFormationContext* FormationContext = FormationGroupComponent->GetContext();
	if (FormationContext && FormationContext->IsFull())
	{
		return;
	}
	
	Super::BeginSpawnActor_Implementation();
}

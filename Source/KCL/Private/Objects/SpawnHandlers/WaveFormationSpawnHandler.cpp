// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Objects/SpawnHandlers/WaveFormationSpawnHandler.h"

#include "Data/FormationDataAssets/FormationDataAsset.h"
#include "Objects/FormationContext.h"

#if WITH_EDITOR
#include "Components/FormationGroupComponent.h"
#include "Misc/DataValidation.h"

EDataValidationResult UWaveFormationSpawnHandler::IsDataValid(class FDataValidationContext& Context) const
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
#endif // WITH_EDITOR

void UWaveFormationSpawnHandler::Activate()
{
	const AActor* OuterActor = Cast<AActor>(GetOuter());
	check(OuterActor);
	
	FormationGroupComponent = OuterActor->GetComponentByClass<UFormationGroupComponent>();
	check(FormationGroupComponent);
	
	Super::Activate();
}

FTransform UWaveFormationSpawnHandler::GetSpawnActorTransform_Implementation() const
{
	check(FormationGroupComponent);
	return FormationGroupComponent->GetNextUnitWorldTransform();
}

void UWaveFormationSpawnHandler::BeginSpawnActor_Implementation()
{
	const UFormationContext* FormationContext = FormationGroupComponent->GetContext();
	if (!FormationContext) return;
	if (FormationContext->IsFull()) return;
	const UFormationDataAsset* FormationDataAsset = FormationContext->GetFormationDataAsset();
	if (!FormationDataAsset) return;
	
	int DesiredCount = FormationDataAsset->GetFormationLimit();
	const int Delta = BatchSpawnData.SpawnCount - SpawnedCount;
	if (Delta < DesiredCount)
	{
		DesiredCount = Delta;
	}

	FTransform SpawnActorTransform = GetSpawnActorTransform();
	TArray<FTransform> Transforms;
	FormationDataAsset->GetWorldTransforms(DesiredCount, SpawnActorTransform.GetLocation(), SpawnActorTransform.GetRotation().GetForwardVector(), Transforms);
	for (int Index = 0; Index < DesiredCount; Index++)
	{
		SpawnActor(Transforms[Index]);
	}
}

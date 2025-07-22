// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Subsystems/FormationSubsystem.h"
#include "Components/FormationComponent.h"
#include "Objects/FormationContext.h"

UFormationSubsystem* UFormationSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject) { return nullptr; }
	const UWorld* World = WorldContextObject->GetWorld();
	return World ? World->GetSubsystem<UFormationSubsystem>() : nullptr; 
}

UFormationContext* UFormationSubsystem::CreateGroup(const FString& GroupID, UFormationDataAsset* DataAsset, AActor* FormationOwner)
{
	if (!FormationHandles.Contains(GroupID))
	{
		UFormationContext* FormationContext = NewObject<UFormationContext>(this);
		FormationContext->SetFormationDataAsset(DataAsset ? DataAsset : GetMutableDefault<UFormationDataAsset>());
		FormationContext->SetFormationOwner(FormationOwner);
		FormationHandles.Add(GroupID, FormationContext);
	}

	return FormationHandles[GroupID];
}

bool UFormationSubsystem::DestroyGroup(const FString& GroupID)
{
	if (FormationHandles.Contains(GroupID))
	{
		FormationHandles[GroupID]->ForEachUnit([&](UFormationComponent* Unit)
		{
			Unit->LeaveFormation();
		});
	}
	return FormationHandles.Remove(GroupID) > 0;
}
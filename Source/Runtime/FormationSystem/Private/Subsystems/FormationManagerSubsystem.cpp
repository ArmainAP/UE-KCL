// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Subsystems/FormationManagerSubsystem.h"

#include "Logging.h"
#include "Data/FormationGroupInfo.h"

UFormationGroupInfo* UFormationManagerSubsystem::FindOrAddFormation(FString FormationID)
{
	if (FormationID.IsEmpty())
	{
		FormationID = FGuid::NewGuid().ToString();
	}
	
	if (!Formations.Contains(FormationID))
	{
		UFormationGroupInfo* FormationGroupInfo = NewObject<UFormationGroupInfo>(this, *FormationID);
		Formations.Add(FormationID, FormationGroupInfo);	
	}

	return Formations[FormationID];
}

bool UFormationManagerSubsystem::RemoveFormation(FString FormationID)
{
	if (!Formations.Contains(FormationID))
	{
		return false;
	}

	Formations[FormationID]->StopMovement();
	Formations.Remove(FormationID);
	return true;
}

void UFormationManagerSubsystem::ClearFormations()
{
	for (auto FormationTuple : Formations)
	{
		if (!IsValid(FormationTuple.Value))
		{
			continue;
		}
		FormationTuple.Value->StopMovement();
	}
	Formations.Empty();
}

// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Subsystems/FormationManagerSubsystem.h"

#include "Logging.h"
#include "Data/FormationGroupInfo.h"


namespace FormationManagerSubsystem
{
	static bool bEnable = false;
	static FAutoConsoleVariableRef CVarEnableFormationManagerSubsystem(
		TEXT("KCL.EnableFormationManagerSubsystem"),
		bEnable,
		TEXT("Enable or disable the Formation Manager Subsystem"),
		ECVF_Default);
}

bool UFormationManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const bool bShouldCreateSubsystem = Super::ShouldCreateSubsystem(Outer) && FormationManagerSubsystem::bEnable;
	UE_LOG(LogFormationSystem, Display, TEXT("%s returned %d"), StringCast<TCHAR>(__FUNCTION__).Get(), bShouldCreateSubsystem);
	return bShouldCreateSubsystem;
}

void UFormationManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogFormationSystem, Display, TEXT("%s executed."), StringCast<TCHAR>(__FUNCTION__).Get());
}

void UFormationManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();

	UE_LOG(LogFormationSystem, Display, TEXT("%s executed."), StringCast<TCHAR>(__FUNCTION__).Get());
}

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

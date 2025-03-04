// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Subsystems/FormationManagerSubsystem.h"

#include "Logging.h"


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

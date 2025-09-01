// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Subsystem/GlobalEventBusSubsystem.h"

UGlobalEventBusSubsystem* UGlobalEventBusSubsystem::Get(const UObject* WorldContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	const UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return nullptr;
	}
	
	return GameInstance->GetSubsystem<UGlobalEventBusSubsystem>();
}


void UGlobalEventBusSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	IConsoleManager& ConsoleManager = IConsoleManager::Get();
	ConsoleManager.RegisterConsoleCommand(TEXT("InvokeGlobalEvent"), TEXT("Invoke a global event by its ID."),
		FConsoleCommandWithArgsDelegate::CreateUObject(this, &UGlobalEventBusSubsystem::HandleInvokeEvent), ECVF_Cheat);
}

void UGlobalEventBusSubsystem::Deinitialize()
{
	RemoveAllListeners();
	EventAssetsByID.Reset();
	
	Super::Deinitialize();
}

bool UGlobalEventBusSubsystem::AddListenerByClass(TSubclassOf<UEventBusDataAsset> AssetClass, const FGlobalEvent& Callback)
{
	return IsValid(AssetClass) ? AddListenerByAsset(GetDefault<UEventBusDataAsset>(AssetClass), Callback) : false;
}

bool UGlobalEventBusSubsystem::AddListenerByAsset(const UEventBusDataAsset* Asset, const FGlobalEvent& Callback)
{
	if(!IsValid(Asset))
	{
		return false;
	}

	RegisterEventAsset(Asset);
	return AddListenerByID(Asset->GetTaskID(), Callback);
}

bool UGlobalEventBusSubsystem::AddListenerByID(FGuid ID, const FGlobalEvent& Callback)
{
	if(!ID.IsValid())
	{
		return false;
	}

	FGlobalEventArray& EventArray = GlobalEventsByID.FindOrAdd(ID);
	EventArray.Add(Callback);
	return true;
}

bool UGlobalEventBusSubsystem::RemoveListenersByClass(const TSubclassOf<UEventBusDataAsset> AssetClass)
{
	return IsValid(AssetClass) ? RemoveListenersByAsset(GetDefault<UEventBusDataAsset>(AssetClass)) : false;
}

bool UGlobalEventBusSubsystem::RemoveListenersByAsset(const UEventBusDataAsset* Asset)
{
	return IsValid(Asset) ? RemoveListenersByID(Asset->GetTaskID()) : false;
}

bool UGlobalEventBusSubsystem::RemoveListenersByID(const FGuid ID)
{
	return GlobalEventsByID.Remove(ID) > 0;
}

void UGlobalEventBusSubsystem::RemoveAllListeners()
{
	GlobalEventsByID.Reset();
}

bool UGlobalEventBusSubsystem::InvokeEventByClass(TSubclassOf<UEventBusDataAsset> AssetClass)
{
	return AssetClass ? InvokeEventByAsset(GetDefault<UEventBusDataAsset>(AssetClass)) : false;
}

bool UGlobalEventBusSubsystem::InvokeEventByAsset(const UEventBusDataAsset* Asset)
{
	return IsValid(Asset) ? InvokeEventByID(Asset->GetTaskID()) : false;
}

bool UGlobalEventBusSubsystem::InvokeEventByID(const FGuid ID)
{
	if (!ID.IsValid())
	{
		return false;
	}

	const UEventBusDataAsset* Context = GetRegisteredAssetByID(ID);
	return BroadcastByID(ID, Context);
}

const UEventBusDataAsset* UGlobalEventBusSubsystem::GetRegisteredAssetByID(FGuid ID) const
{
	if (!ID.IsValid())
	{
		return nullptr;
	}
	
	if (const FGlobalEventSet* Set = EventAssetsByID.Find(ID))
	{
		// Return the first valid asset; opportunistically prune dead refs
		for (const TWeakObjectPtr<const UEventBusDataAsset>& Weak : *Set)
		{
			if (Weak.IsValid())
			{
				return Weak.Get();
			}
		}
	}
	return nullptr;
}

void UGlobalEventBusSubsystem::RegisterEventAsset(const UEventBusDataAsset* Asset)
{
	if (!IsValid(Asset))
	{
		return;
	}
	
	const FGuid ID = Asset->GetTaskID();
	if (!ID.IsValid())
	{
		return;
	}

	FGlobalEventSet& Set = EventAssetsByID.FindOrAdd(ID);
	Set.Add(Asset);
}

void UGlobalEventBusSubsystem::UnregisterEventAsset(const UEventBusDataAsset* Asset)
{
	if (!Asset)
	{
		return;
	}
	
	const FGuid ID = Asset->GetTaskID();
	if (!ID.IsValid())
	{
		return;
	}

	if (FGlobalEventSet* Set = EventAssetsByID.Find(ID))
	{
		// Remove matching entries and dead weak refs
		TArray<TWeakObjectPtr<const UEventBusDataAsset>> ToRemove;
		for (const TWeakObjectPtr<const UEventBusDataAsset>& Weak : *Set)
		{
			if (!Weak.IsValid() || Weak.Get() == Asset)
			{
				ToRemove.Add(Weak);
			}
		}
		
		for (const auto& W : ToRemove)
		{
			Set->Remove(W);
		}

		if (Set->Num() == 0)
		{
			EventAssetsByID.Remove(ID);
		}
	}
}

void UGlobalEventBusSubsystem::HandleInvokeEvent(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("InvokeGlobalEvent: Missing event ID argument."));
		return;
	}

	const FString EventID = Args[0];
	FGuid NewGuid;
	FGuid::Parse(EventID, NewGuid);
	if (InvokeEventByID(NewGuid))
	{
		UE_LOG(LogTemp, Log, TEXT("InvokeGlobalEvent: Successfully invoked event with ID '%s'"), *EventID);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InvokeGlobalEvent: Failed to invoke event with ID '%s'"), *EventID);
	}
}

bool UGlobalEventBusSubsystem::BroadcastByID(const FGuid& ID, const UEventBusDataAsset* ContextAsset)
{
	if (!ID.IsValid())
	{
		return false;
	}

	bool bAny = false;
	if (const FGlobalEventArray* Arr = GlobalEventsByID.Find(ID))
	{
		for (const FGlobalEvent& Ev : *Arr)
		{
			bAny |= Ev.ExecuteIfBound(ContextAsset);
		}
	}
	return bAny;
}

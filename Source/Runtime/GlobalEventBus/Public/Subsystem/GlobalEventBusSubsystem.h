// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "Data/EventBusDataAsset.h"
#include "GlobalEventBusSubsystem.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FGlobalEvent, const UEventBusDataAsset*, Event);

UCLASS()
class GLOBALEVENTBUS_API UGlobalEventBusSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UGlobalEventBusSubsystem* Get(const UObject* WorldContextObject);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	// Add Listeners
	UFUNCTION(BlueprintCallable, Category = "Global Event Bus")
	bool AddListenerByClass(TSubclassOf<UEventBusDataAsset> AssetClass, const FGlobalEvent& Callback);

	UFUNCTION(BlueprintCallable, Category = "Global Event Bus")
	bool AddListenerByAsset(const UEventBusDataAsset* Asset, const FGlobalEvent& Callback);

	UFUNCTION(BlueprintCallable, Category = "Global Event Bus")
	bool AddListenerByID(FGuid ID, const FGlobalEvent& Callback);

	// Remove Listeners
	UFUNCTION(BlueprintCallable, Category = "Global Event Bus")
	bool RemoveListenersByClass(TSubclassOf<UEventBusDataAsset> AssetClass);

	UFUNCTION(BlueprintCallable, Category = "Global Event Bus")
	bool RemoveListenersByAsset(const UEventBusDataAsset* Asset);

	UFUNCTION(BlueprintCallable, Category = "Global Event Bus")
	bool RemoveListenersByID(FGuid ID);

	UFUNCTION(BlueprintCallable, Category = "Global Event Bus")
	void RemoveAllListeners();

	// Invoke Events
	UFUNCTION(BlueprintCallable, Category = "Global Event Bus")
	bool InvokeEventByClass(TSubclassOf<UEventBusDataAsset> AssetClass);

	UFUNCTION(BlueprintCallable, Category = "Global Event Bus")
	bool InvokeEventByAsset(const UEventBusDataAsset* Asset);

	UFUNCTION(BlueprintCallable, Category = "Global Event Bus")
	bool InvokeEventByID(FGuid ID);

	UFUNCTION(BlueprintPure, Category="Global Event Bus|Registry")
	const UEventBusDataAsset* GetRegisteredAssetByID(FGuid ID) const;

protected:
	UFUNCTION(BlueprintCallable, Category="Global Event Bus|Registry")
	void RegisterEventAsset(const UEventBusDataAsset* Asset);

	UFUNCTION(BlueprintCallable, Category="Global Event Bus|Registry")
	void UnregisterEventAsset(const UEventBusDataAsset* Asset);
	
private:
	void HandleInvokeEvent(const TArray<FString>& Args);

	bool BroadcastByID(const FGuid& ID, const UEventBusDataAsset* ContextAsset);

protected:
	typedef TArray<FGlobalEvent> FGlobalEventArray;
	TMap<FGuid, FGlobalEventArray> GlobalEventsByID;

	typedef TSet<TWeakObjectPtr<const UEventBusDataAsset>, TWeakObjectPtrSetKeyFuncs<TWeakObjectPtr<const UEventBusDataAsset>>> FGlobalEventSet;
	TMap<FGuid, FGlobalEventSet> EventAssetsByID;
};
// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BlueprintWorldSubsystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class BLUEPRINTSUBSYSTEMS_API UBlueprintWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void OnWorldComponentsUpdated(UWorld& World) override;

	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnInitialize();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeinitialize();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPostInitialize();

	UFUNCTION(BlueprintImplementableEvent)
	void OnWorldBeginPlayEvent(UWorld* InWorld);

	UFUNCTION(BlueprintImplementableEvent)
	void OnWorldComponentsUpdatedEvent(UWorld* World);
};

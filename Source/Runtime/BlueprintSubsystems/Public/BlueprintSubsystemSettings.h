// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintGameInstanceSubsystem.h"
#include "BlueprintLocalPlayerSubsystem.h"
#include "BlueprintWorldSubsystem.h"
#include "Runtime/DeveloperSettings/Public/Engine/DeveloperSettings.h"
#include "BlueprintSubsystemSettings.generated.h"

UCLASS(Config=Game, DefaultConfig)
class BLUEPRINTSUBSYSTEMS_API UBlueprintSubsystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(Config, EditAnywhere)
	TArray<TSubclassOf<UBlueprintLocalPlayerSubsystem>> BlueprintLocalPlayerSubsystems;

	UPROPERTY(Config, EditAnywhere)
	TArray<TSubclassOf<UBlueprintGameInstanceSubsystem>> BlueprintGameInstanceSubsystems;

	UPROPERTY(Config, EditAnywhere)
	TArray<TSubclassOf<UBlueprintWorldSubsystem>> BlueprintWorldSubsystems;
};

UCLASS()
class UBlueprintLocalPlayerSubsystemInitializer : public ULocalPlayerSubsystem 
{
	GENERATED_BODY()
	virtual void Initialize(FSubsystemCollectionBase& Collection) override
	{
		Super::Initialize(Collection);
		
		const UBlueprintSubsystemSettings* Settings = GetDefault<UBlueprintSubsystemSettings>();
		if (!Settings->IsValidLowLevelFast()) return;
		for(auto Subsystem : Settings->BlueprintLocalPlayerSubsystems)
		{
			Collection.InitializeDependency(Subsystem);
		}
	}
};

UCLASS()
class UBlueprintGameInstanceSubsystemInitializer : public UGameInstanceSubsystem 
{
	GENERATED_BODY()
	virtual void Initialize(FSubsystemCollectionBase& Collection) override
	{
		Super::Initialize(Collection);
		
		const UBlueprintSubsystemSettings* Settings = GetDefault<UBlueprintSubsystemSettings>();
		if (!Settings->IsValidLowLevelFast()) return;
		for(auto Subsystem : Settings->BlueprintGameInstanceSubsystems)
		{
			Collection.InitializeDependency(Subsystem);
		}
	}
};

UCLASS()
class UBlueprintWorldSubsystemInitializer : public UWorldSubsystem 
{
	GENERATED_BODY()
	virtual void Initialize(FSubsystemCollectionBase& Collection) override
	{
		Super::Initialize(Collection);
		
		const UBlueprintSubsystemSettings* Settings = GetDefault<UBlueprintSubsystemSettings>();
		if (!Settings->IsValidLowLevelFast()) return;
		for(auto Subsystem : Settings->BlueprintWorldSubsystems)
		{
			Collection.InitializeDependency(Subsystem);
		}
	}
};
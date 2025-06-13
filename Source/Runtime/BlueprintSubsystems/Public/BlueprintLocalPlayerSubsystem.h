// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "BlueprintLocalPlayerSubsystem.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class BLUEPRINTSUBSYSTEMS_API UBlueprintLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void PlayerControllerChanged(APlayerController* NewPlayerController);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnInitialize();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeinitialize();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerControllerChanged(APlayerController* NewPlayerController);
};
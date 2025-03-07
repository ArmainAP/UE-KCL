// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintWorldSubsystem.h"
#include "Subsystems/WorldSubsystem.h"
#include "BlueprintTickableWorldSubsystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class BLUEPRINTSUBSYSTEMS_API UBlueprintTickableWorldSubsystem : public UBlueprintWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	
	virtual ETickableTickType GetTickableTickType() const override
	{
		return ETickableTickType::Conditional;
	}
	
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UBlueprintTickableWorldSubsystem, STATGROUP_Tickables);
	}
	
	virtual bool IsTickable() const override
	{
		return bTickable;
	}

	virtual bool IsTickableWhenPaused() const override
	{
		return bTickableWhenPaused;
	}

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnTick(float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTickable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTickableWhenPaused = true;

private:
	// The last frame number we were ticked.
	// We don't want to tick multiple times per frame 
	uint32 LastFrameNumberWeTicked = INDEX_NONE;
};
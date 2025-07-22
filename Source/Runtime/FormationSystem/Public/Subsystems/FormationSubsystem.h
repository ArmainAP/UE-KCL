// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FormationSubsystem.generated.h"

class UFormationContext;
class UFormationDataAsset;



/**
 * 
 */
UCLASS()
class FORMATIONSYSTEM_API UFormationSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UFormationSubsystem* Get(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable)
	UFormationContext* CreateGroup(const FString& GroupID, UFormationDataAsset* DataAsset,
	                               AActor* FormationOwner = nullptr);

	UFUNCTION(BlueprintCallable)
	bool DestroyGroup(const FString& GroupID);
		
protected:
	TMap<const FString, TObjectPtr<UFormationContext>> FormationHandles;
};
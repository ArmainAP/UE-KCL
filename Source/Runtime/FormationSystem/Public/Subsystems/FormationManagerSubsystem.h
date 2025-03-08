// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FormationManagerSubsystem.generated.h"

class UFormationGroupInfo;
/**
 * 
 */
UCLASS()
class FORMATIONSYSTEM_API UFormationManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UFormationGroupInfo* FindOrAddFormation(FString FormationID);

	UFUNCTION(BlueprintCallable)
	bool RemoveFormation(FString FormationID);

	UFUNCTION(BlueprintCallable)
	void ClearFormations();

protected:
	UPROPERTY()
	TMap<FString, UFormationGroupInfo*> Formations;
};

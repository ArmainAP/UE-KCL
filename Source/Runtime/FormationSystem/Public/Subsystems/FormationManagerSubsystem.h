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

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	UFUNCTION(BlueprintCallable)
	UFormationGroupInfo* RegisterFormation(FString FormationID);

	UFUNCTION(BlueprintCallable)
	bool RemoveFormation(FString FormationID);

	UFUNCTION(BlueprintCallable)
	void ClearFormations();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<FString, UFormationGroupInfo*> Formations;
};

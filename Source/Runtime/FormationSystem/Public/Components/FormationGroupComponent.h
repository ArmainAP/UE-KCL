// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FormationComponent.h"
#include "Components/ActorComponent.h"
#include "Components/ArrowComponent.h"
#include "FormationGroupComponent.generated.h"

UCLASS( BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent), Category="Components|FormationSystem" )
class FORMATIONSYSTEM_API UFormationGroupComponent : public UArrowComponent
{
	GENERATED_BODY()

public:	
	UFormationGroupComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure)
	bool GetUnits(TArray<UFormationComponent*>& Units) const;
	
	UFUNCTION(BlueprintPure)
	FName GetFormationID() const;

	UFUNCTION(BlueprintPure)
	int GetUnitsCount() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Direction = FVector::ForwardVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseWorldDirection = false;

	UPROPERTY(BlueprintAssignable)
	FFormationUnitEvent OnUnitJoined;

	UPROPERTY(BlueprintAssignable)
	FFormationUnitEvent OnUnitLeft;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UFormationDataAsset* DefaultFormationDataAsset = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName FormationID;

	UPROPERTY()
	TObjectPtr<class UFormationSubsystem> CachedFormationSubsystem;
};

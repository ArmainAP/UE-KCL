// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/ArrowComponent.h"
#include "Interfaces/FormationUnit.h"
#include "Objects/FormationGroupInfo.h"
#include "FormationGroupComponent.generated.h"


class UFormationDataAsset;
class UFormationGroupInfo;

UCLASS( BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent), Category="Components|FormationSystem" )
class FORMATIONSYSTEM_API UFormationGroupComponent : public UArrowComponent
{
	GENERATED_BODY()

public:	
	UFormationGroupComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UFormationGroupInfo* GetFormationGroup();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Direction = FVector::ForwardVector;
	
	UPROPERTY(BlueprintAssignable)
	FUnitsChanged OnFormationUnitLeft;

	UPROPERTY(BlueprintAssignable)
	FUnitsChanged OnFormationUnitJoined;

protected:
	void MoveToOwner();

	UFUNCTION()
	void OnFormationUnitJoinedInternal(TScriptInterface<IFormationUnit> Unit);

	UFUNCTION()
	void OnFormationUnitLeftInternal(TScriptInterface<IFormationUnit> Unit);
	
	UPROPERTY(BlueprintReadOnly)
	UFormationGroupInfo* FormationGroup = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UFormationDataAsset* DefaultFormationDataAsset = nullptr;
};

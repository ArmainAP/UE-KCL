// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FormationComponent.h"
#include "Components/ActorComponent.h"
#include "Components/ArrowComponent.h"
#include "FormationGroupComponent.generated.h"

class UFormationContext;

UCLASS( BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent), Category="Components|FormationSystem" )
class FORMATIONSYSTEM_API UFormationGroupComponent : public UArrowComponent
{
	GENERATED_BODY()

public:	
	UFormationGroupComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintGetter)
	FString GetID() const { return ID; }

	UFUNCTION(BlueprintGetter)
	UFormationContext* GetContext() const { return Context; }

	UFUNCTION(BlueprintSetter)
	void SetDirection(const FVector& InDirection) { Direction = InDirection; }

	UFUNCTION(BlueprintPure)
	FTransform GetUnitWorldTransform(const int Index) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintSetter=SetDirection)
	FVector Direction = FVector::ForwardVector;

	UPROPERTY(EditAnywhere)
	bool bUseWorldDirection = false;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UFormationDataAsset> DefaultFormationDataAsset = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter=GetID)
	FString ID;

	UPROPERTY(EditDefaultsOnly)
	bool bAutoDestroyFormation = false;

	UPROPERTY(BlueprintGetter=GetContext)
	TObjectPtr<class UFormationContext> Context = nullptr;
};

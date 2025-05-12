// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Data/SightSystemDataAsset.h"
#include "Data/SightSystemMultipliers.h"
#include "SighterComponent.generated.h"

class USightedComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSightedComponent, USightedComponent*, SightedComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSightedComponentInfo, USightedComponent*, SightedComponent, bool, bWasPerceived);

UENUM(BlueprintType, Blueprintable)
enum class ESightedState : uint8
{
	NotPerceived,
	Spotted,
	Perceived,
	Lost,
};

UCLASS(Blueprintable, BlueprintType, ClassGroup=(SightSystem), meta=(BlueprintSpawnableComponent) )
class SIGHTSYSTEM_API USighterComponent : public USceneComponent
{
	GENERATED_BODY()

	friend class USightQueryManager;
	
public:
	// Sets default values for this component's properties
	USighterComponent();

	UFUNCTION(BlueprintPure, Category="Sight System")
	void GetSightedComponents(ESightedState SightedState, TArray<USightedComponent*>& OutSightedComponent) const;

	UFUNCTION(BlueprintPure, Category="Sight System")
	bool HasSightedComponent(ESightedState SightedState, USightedComponent* SightedComponent) const;

	UFUNCTION(BlueprintPure, Category="Sight System")
	bool HasAnySightedComponent(ESightedState SightedState) const;

	UFUNCTION(BlueprintPure, Category="Sight System")
	ESightedState GetStateForSightedComponent(const USightedComponent* TargetActor) const;

protected:
	virtual void BeginPlay() override;
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;
	
	void SpotTarget(USightedComponent* TargetActor);
	void PerceiveTarget(USightedComponent* TargetActor, bool bWasRemembered);
	void StartTargetLostWait(USightedComponent* TargetActor);
	void LoseTarget(USightedComponent* TargetActor, bool bWasActorPerceived);
	void ForgetTarget(USightedComponent* TargetActor);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	FSightSystemMultipliers SightSystemMultipliers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	TObjectPtr<USightSystemDataAsset> SightDataAsset;

	UPROPERTY(BlueprintAssignable)
	FOnSightedComponent OnSpotted;

	UPROPERTY(BlueprintAssignable)
	FOnSightedComponentInfo OnPerceived;

	UPROPERTY(BlueprintAssignable)
	FOnSightedComponentInfo OnLost;

	UPROPERTY(BlueprintAssignable)
	FOnSightedComponent OnForgotten;

	UPROPERTY(BlueprintAssignable)
	FOnSightedComponent OnLostGracePeriodBegin;

protected:
	UPROPERTY()
	TArray<TObjectPtr<USightedComponent>> PerceivedSightedComponents;

	UPROPERTY()
	TArray<TObjectPtr<USightedComponent>> SpottedSightedComponents;

	UPROPERTY()
	TArray<TObjectPtr<USightedComponent>> LostSightedComponents;
};

// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SighterComponent.h"
#include "Data/SightQueryContext.h"
#include "Subsystems/WorldSubsystem.h"
#include "SightQueryManager.generated.h"

class USightedComponent;

/**
 * 
 */
UCLASS()
class SIGHTSYSTEM_API USightQueryManager : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { return TStatId(); };
	
	UFUNCTION(BlueprintPure, Category = "Sight System")
	bool IsSightedRegistered(const USightedComponent* SightedComponent) const;
	
	UFUNCTION(BlueprintCallable, Category = "Sight System")
	void RegisterSighted(USightedComponent* SightedComponent);
	
	UFUNCTION(BlueprintCallable, Category = "Sight System")
	void UnregisterSighted(USightedComponent* SightedComponent);

	UFUNCTION(BlueprintPure, Category = "Sight System")
	bool IsSighterRegistered(const USighterComponent* SighterComponent) const;
	
	UFUNCTION(BlueprintCallable, Category = "Sight System")
	void RegisterSighter(USighterComponent* SighterComponent);
	
	UFUNCTION(BlueprintCallable, Category = "Sight System")
	void UnregisterSighter(USighterComponent* SighterComponent);

	UFUNCTION(BlueprintPure, Category = "Sight System")
	float GetGainValueForTarget(USighterComponent* SighterComponent, USightedComponent* SightedComponent) const;

	UFUNCTION(BlueprintPure, Category = "Sight System")
	const USightedComponent* GetHighestGainValue(USighterComponent* SighterComponent, float& OutGain) const;

	UFUNCTION(BlueprintPure, Category = "Sight System")
	FVector GetLastKnownLocationFor(USighterComponent* SighterComponent, USightedComponent* SightedComponent) const;

	UFUNCTION(BlueprintCallable, Category = "Sight System")
	void AddGainToTarget(USighterComponent* SighterComponent, USightedComponent* SightedComponent, float Value);
	
	UFUNCTION(BlueprintCallable, Category = "Sight System")
	void ForcePerceiveTarget(USighterComponent* SighterComponent, USightedComponent* SightedComponent);

	UFUNCTION(BlueprintCallable, Category = "Sight System")
	void ForceForgetTarget(USighterComponent* SighterComponent, USightedComponent* SightedComponent);

	UFUNCTION(BlueprintCallable, Category = "Sight System")
	void ForceForgetAllTargets(USighterComponent* SighterComponent);
	
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = "Sight System")
	bool IsSpotted(USightedComponent* SightedComponent, TArray<USighterComponent*>& OutSighters) const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = "Sight System")
	bool IsPerceived(USightedComponent* SightedComponent, TArray<USighterComponent*>& OutSighters) const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = "Sight System")
	bool IsLost(USightedComponent* SightedComponent, TArray<USighterComponent*>& OutSighters) const;

protected:
	void GetSightersByCondition(USightedComponent* SightedComponent, TArray<USighterComponent*>& OutSighters, const TFunction<bool(const FSightQueryContext&)>& Condition) const;

	static void HandleSuccessfulCheck(FSightQueryContext& Q, float Dt, const FVector& CurrentPos);
	static void HandlePerceivedLoss(FSightQueryContext& Q, float Dt, const FVector& CurrentPos);
	static void HandleSpottedLoss(FSightQueryContext& Q, float Dt);
	static void ProcessQueryState(FSightQueryContext& Q, float Dt);

	TSet<TWeakObjectPtr<USighterComponent>> SighterComponents;
	TSet<TWeakObjectPtr<USightedComponent>> SightedComponents;
	TArray<FSightQueryContext> SightQueries;
};

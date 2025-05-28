// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "LogicController.generated.h"

/**
 * 
 */
UCLASS()
class KCL_API ALogicController : public AAIController
{
	GENERATED_BODY()

public:
	ALogicController();
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StartAILogic(APawn* InNewPawn);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StopAILogic();

	UFUNCTION(BlueprintNativeEvent)
	void RunLogic(const float DeltaTime);
	virtual void RunLogic_Implementation(const float DeltaTime) {};

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UPROPERTY(EditAnywhere)
	bool bShouldRunLogic = false;
};

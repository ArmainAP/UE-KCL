// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FormationUnit.generated.h"

class UFormationGroupInfo;

UINTERFACE(Blueprintable, BlueprintType)
class FORMATIONSYSTEM_API UFormationUnit : public UInterface
{
	GENERATED_BODY()
};

class FORMATIONSYSTEM_API IFormationUnit
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetupTarget(const FVector& InTargetLocation, const FRotator& InTargetRotation);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StopMovement();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool HasReached();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FTransform GetTransform() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HandleFormationLeft(UFormationGroupInfo* OldFormation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HandleFormationJoined(UFormationGroupInfo* NewFormation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AActor* GetActor() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetDistanceToDestination() const;
};

// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/ActorComponent.h"
#include "KiraHelperLibrary.generated.h"


UCLASS()
class KCL_API UKiraHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "KiraHelperLibrary")
	static AAIController* GetAIController(AActor* Actor);
	
	UFUNCTION(BlueprintPure, Category = "KiraHelperLibrary")
	static APawn* GetPawn(AActor* Actor);
};

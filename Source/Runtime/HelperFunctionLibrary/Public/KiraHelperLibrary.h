// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KiraHelperLibrary.generated.h"

class AAIController;

UCLASS()
class HELPERFUNCTIONLIBRARY_API UKiraHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "KiraHelperLibrary")
	static AAIController* GetAIController(AActor* Actor);
	
	UFUNCTION(BlueprintPure, Category = "KiraHelperLibrary")
	static APawn* GetPawn(AActor* Actor);

	UFUNCTION(BlueprintPure, Category = "KiraHelperLibrary")
	static bool GetNavigablePathLenght(UWorld* WorldContextObject, const FVector& Start, const FVector& End, float& OutLength);
};

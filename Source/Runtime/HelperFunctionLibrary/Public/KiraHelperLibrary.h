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

	template<class T>
	static T* GetAIController(AActor* Actor)
	{
		return Cast<T>(GetAIController(Actor));
	}
	
	UFUNCTION(BlueprintPure, Category = "KiraHelperLibrary")
	static APawn* GetPawn(AActor* Actor);

	template<class T>
	static T* GetPawn(AActor* Actor)
	{
		return Cast<T>(GetPawn(Actor));
	}

	UFUNCTION(BlueprintPure, Category = "KiraHelperLibrary")
	static bool GetNavigablePathLenght(UWorld* WorldContextObject, const FVector& Start, const FVector& End, float& OutLength);
};

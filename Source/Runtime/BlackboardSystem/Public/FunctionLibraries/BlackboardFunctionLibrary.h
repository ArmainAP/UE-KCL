// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Subsystems/BlackboardSubsystem.h"
#include "BlackboardFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class BLACKBOARDSYSTEM_API UBlackboardFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static UBlackboardSubsystem* GetBlackboardSubsystem(const UWorld* Owner);
	
	UFUNCTION(BlueprintPure)
	static UBlackboardContext* GetBlackboardContext(UObject* Object);

	UFUNCTION(BlueprintPure, meta=(DeterminesOutputType="Class"))
	static UObject* GetValueObject(UObject* Object, FName Name, const UClass* Class = nullptr);

	UFUNCTION(BlueprintCallable)
	static bool SetValueObject(UObject* Object, FName Name, UObject* ValueObject, UClass* Class = nullptr);

	UFUNCTION(BlueprintPure)
	static FVector GetValueVector(UObject* Object, const FName Name);

	UFUNCTION(BlueprintCallable)
	static bool SetValueVector(UObject* Object, FName Name, const FVector& Value);
};

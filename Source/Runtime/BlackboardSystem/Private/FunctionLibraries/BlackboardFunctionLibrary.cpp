// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "FunctionLibraries/BlackboardFunctionLibrary.h"

UBlackboardSubsystem* UBlackboardFunctionLibrary::GetBlackboardSubsystem(const UWorld* WorldContext)
{
	if (!IsValid(WorldContext))
	{
		return nullptr;
	}

	if (const UGameInstance* GameInstance = WorldContext->GetGameInstance())
	{
		return IsValid(GameInstance) ? GameInstance->GetSubsystem<UBlackboardSubsystem>() : nullptr;
	}
	
	return nullptr;
}

UBlackboardContext* UBlackboardFunctionLibrary::GetBlackboardContext(UObject* Object)
{
	const UWorld* WorldContext = IsValid(Object) ? Object->GetWorld() : nullptr;
	UBlackboardSubsystem* BlackboardSubsystem = GetBlackboardSubsystem(WorldContext);
	return IsValid(BlackboardSubsystem) ? BlackboardSubsystem->GetBlackboardContext(Object) : nullptr;
}

UObject* UBlackboardFunctionLibrary::GetValueObject(UObject* Object, const FName Name, const UClass* Class)
{
	if (const UBlackboardContext* BlackboardContext = GetBlackboardContext(Object))
	{
		return BlackboardContext->GetValueObject(Name, Class);
	}
	return nullptr;
}

bool UBlackboardFunctionLibrary::SetValueObject(UObject* Object, FName Name, UObject* ValueObject, UClass* Class)
{
	if (UBlackboardContext* BlackboardContext = GetBlackboardContext(Object))
	{
		return BlackboardContext->SetValueObject(Name, ValueObject, Class) == EPropertyBagResult::Success;
	}
	return false;
}


FVector UBlackboardFunctionLibrary::GetValueVector(UObject* Object, const FName Name)
{
	if (const UBlackboardContext* BlackboardContext = GetBlackboardContext(Object))
	{
		const FInstancedStruct InstancedStruct = BlackboardContext->GetValueStruct(Name);
		return InstancedStruct.IsValid() ? InstancedStruct.Get<FVector>() : FVector::ZeroVector;
	}
	return FVector::ZeroVector;
}

bool UBlackboardFunctionLibrary::SetValueVector(UObject* Object, const FName Name, const FVector& Value)
{
	if (UBlackboardContext* BlackboardContext = GetBlackboardContext(Object))
	{
		return BlackboardContext->SetValueStruct(Name, FInstancedStruct::Make(Value)) == EPropertyBagResult::Success;
	}
	return false;
}

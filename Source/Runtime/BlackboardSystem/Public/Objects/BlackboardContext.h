// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "StructUtils/PropertyBag.h"
#include "BlackboardContext.generated.h"

UCLASS(BlueprintType, Blueprintable)
class BLACKBOARDSYSTEM_API UBlackboardContext : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void AddProperty(FName InName, EPropertyBagPropertyType InValueType);
	
	UFUNCTION(BlueprintCallable)
	void RemovePropertyByName(const FName PropertyToRemove);

	UFUNCTION(BlueprintPure)
	bool GetValueBool(const FName Name) const;

	UFUNCTION(BlueprintPure)
	uint8 GetValueByte(const FName Name) const;

	UFUNCTION(BlueprintPure)
	int32 GetValueInt32(const FName Name) const;

	UFUNCTION(BlueprintPure)
	int64 GetValueInt64(const FName Name) const;

	UFUNCTION(BlueprintPure)
	float GetValueFloat(const FName Name) const;
	
	UFUNCTION(BlueprintPure)
	double GetValueDouble(const FName Name) const;

	UFUNCTION(BlueprintPure)
	FName GetValueName(const FName Name) const;

	UFUNCTION(BlueprintPure)
	FString GetValueString(const FName Name) const;

	UFUNCTION(BlueprintPure)
	FText GetValueText(const FName Name) const;

	UFUNCTION(BlueprintPure)
	uint8 GetValueEnum(const FName Name, const UEnum* RequestedEnum) const;

	UFUNCTION(BlueprintPure)
	FInstancedStruct GetValueStruct(FName Name) const;

	UFUNCTION(BlueprintPure)
	UObject* GetValueObject(const FName Name, const UClass* RequestedClass = nullptr) const;

	UFUNCTION(BlueprintPure)
	UClass* GetValueClass(const FName Name) const;

	UFUNCTION(BlueprintPure)
	FSoftObjectPath GetValueSoftPath(const FName Name) const;

	/** @return string-based serialized representation of the value. */
	UFUNCTION(BlueprintPure)
	FString GetValueSerializedString(const FName Name) const;

	UFUNCTION(BlueprintCallable)
	EPropertyBagResult SetValueBool(const FName Name, const bool bInValue);

	UFUNCTION(BlueprintCallable)
	EPropertyBagResult SetValueByte(const FName Name, const uint8 InValue);

	UFUNCTION(BlueprintCallable)
	EPropertyBagResult SetValueInt32(const FName Name, const int32 InValue);

	UFUNCTION(BlueprintCallable)
	EPropertyBagResult SetValueInt64(const FName Name, const int64 InValue);
	
	UFUNCTION(BlueprintCallable)
	EPropertyBagResult SetValueFloat(const FName Name, const float InValue);

	UFUNCTION(BlueprintCallable)
	EPropertyBagResult SetValueDouble(const FName Name, const double InValue);

	UFUNCTION(BlueprintCallable)
	EPropertyBagResult SetValueName(const FName Name, const FName InValue);

	UFUNCTION(BlueprintCallable)
	EPropertyBagResult SetValueString(const FName Name, const FString& InValue);

	UFUNCTION(BlueprintCallable)
	EPropertyBagResult SetValueText(const FName Name, const FText& InValue);

	UFUNCTION(BlueprintCallable)
	EPropertyBagResult SetValueStruct(const FName Name, FInstancedStruct InValue);

	UFUNCTION(BlueprintCallable)
	EPropertyBagResult SetValueObject(const FName Name, UObject* InValue, UClass* InClass = nullptr);

	UFUNCTION(BlueprintCallable)
	EPropertyBagResult SetValueClass(const FName Name, UClass* InValue);

	UFUNCTION(BlueprintCallable)
	EPropertyBagResult SetValueSoftPath(const FName Name, const FSoftObjectPath& InValue);

	/**
	 * Sets property value from a serialized representation of the value. If the string value provided
	 * cannot be parsed by the property, the operation will fail.
	 */
	UFUNCTION(BlueprintCallable)
	EPropertyBagResult SetValueSerializedString(const FName Name, const FString& InValue);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnPropertyChanged, FName);
	FOnPropertyChanged OnPropertyChanged;

private:
	UPROPERTY()
	FInstancedPropertyBag PropertyBag;
};

// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Objects/BlackboardContext.h"

void UBlackboardContext::AddProperty(const FName InName, const EPropertyBagPropertyType InValueType)
{
	PropertyBag.AddProperty(InName, InValueType);
}

void UBlackboardContext::RemovePropertyByName(const FName PropertyToRemove)
{
	PropertyBag.RemovePropertyByName(PropertyToRemove);
}

bool UBlackboardContext::GetValueBool(const FName Name) const
{
	bool ReturnValue = false;
	auto Value = PropertyBag.GetValueBool(Name);
	if (Value.HasValue())
	{
		ReturnValue = Value.GetValue();
	}
	return ReturnValue;
}

uint8 UBlackboardContext::GetValueByte(const FName Name) const
{
	uint8 ReturnValue = 0;
	auto Value = PropertyBag.GetValueByte(Name);
	if (Value.HasValue())
	{
		ReturnValue = Value.GetValue();
	}
	return ReturnValue;
}

int32 UBlackboardContext::GetValueInt32(const FName Name) const
{
	int32 ReturnValue = 0;
	auto Value = PropertyBag.GetValueInt32(Name);
	if (Value.HasValue())
	{
		ReturnValue = Value.GetValue();
	}
	return ReturnValue;
}

int64 UBlackboardContext::GetValueInt64(const FName Name) const
{
	int64 ReturnValue = 0;
	auto Value = PropertyBag.GetValueInt64(Name);
	if (Value.HasValue())
	{
		ReturnValue = Value.GetValue();
	}
	return ReturnValue;
}

float UBlackboardContext::GetValueFloat(const FName Name) const
{
	float ReturnValue = false;
	auto Value = PropertyBag.GetValueFloat(Name);
	if (Value.HasValue())
	{
		ReturnValue = Value.GetValue();
	}
	return ReturnValue;
}

double UBlackboardContext::GetValueDouble(const FName Name) const
{
	double ReturnValue = 0;
	auto Value = PropertyBag.GetValueDouble(Name);
	if (Value.HasValue())
	{
		ReturnValue = Value.GetValue();
	}
	return ReturnValue;
}

FName UBlackboardContext::GetValueName(const FName Name) const
{
	FName ReturnValue;
	auto Value = PropertyBag.GetValueName(Name);
	if (Value.HasValue())
	{
		ReturnValue = Value.GetValue();
	}
	return ReturnValue;
}

FString UBlackboardContext::GetValueString(const FName Name) const
{
	FString ReturnValue;
	auto Value = PropertyBag.GetValueString(Name);
	if (Value.HasValue())
	{
		ReturnValue = Value.GetValue();
	}
	return ReturnValue;
}

FText UBlackboardContext::GetValueText(const FName Name) const
{
	FText ReturnValue;
	auto Value = PropertyBag.GetValueText(Name);
	if (Value.HasValue())
	{
		ReturnValue = Value.GetValue();
	}
	return ReturnValue;
}

uint8 UBlackboardContext::GetValueEnum(const FName Name, const UEnum* RequestedEnum) const
{
	uint8 ReturnValue = false;
	auto Value = PropertyBag.GetValueEnum(Name, RequestedEnum);
	if (Value.HasValue())
	{
		ReturnValue = Value.GetValue();
	}
	return ReturnValue;
}

FInstancedStruct UBlackboardContext::GetValueStruct(const FName Name) const
{
	FInstancedStruct ReturnValue;
	auto Value = PropertyBag.GetValueStruct(Name);
	if (Value.HasValue())
	{
		ReturnValue = Value.GetValue();
	}
	return ReturnValue;
}

UObject* UBlackboardContext::GetValueObject(const FName Name, const UClass* RequestedClass) const
{
	UObject* ReturnValue = nullptr;
	auto Value = PropertyBag.GetValueObject(Name, RequestedClass);
	if (Value.HasValue())
	{
		ReturnValue = Value.GetValue();
	}
	return ReturnValue;
}

UClass* UBlackboardContext::GetValueClass(const FName Name) const
{
	UClass* ReturnValue = nullptr;
	auto Value = PropertyBag.GetValueClass(Name);
	if (Value.HasValue())
	{
		ReturnValue = Value.GetValue();
	}
	return ReturnValue;
}

FSoftObjectPath UBlackboardContext::GetValueSoftPath(const FName Name) const
{
	FSoftObjectPath ReturnValue;
	auto Value = PropertyBag.GetValueSoftPath(Name);
	if (Value.HasValue())
	{
		ReturnValue = Value.GetValue();
	}
	return ReturnValue;
}

FString UBlackboardContext::GetValueSerializedString(const FName Name) const
{
	FString ReturnValue;
	auto Value = PropertyBag.GetValueSerializedString(Name);
	if (Value.HasValue())
	{
		ReturnValue = Value.GetValue();
	}
	return ReturnValue;
}

EPropertyBagResult UBlackboardContext::SetValueBool(const FName Name, const bool bInValue)
{
	PropertyBag.AddProperty(Name, EPropertyBagPropertyType::Bool);
	const EPropertyBagResult Result = PropertyBag.SetValueBool(Name, bInValue);
	OnPropertyChanged.Broadcast(Name);
	return Result;
}

EPropertyBagResult UBlackboardContext::SetValueByte(const FName Name, const uint8 InValue)
{
	PropertyBag.AddProperty(Name, EPropertyBagPropertyType::Byte);
	const EPropertyBagResult Result = PropertyBag.SetValueByte(Name, InValue);
	OnPropertyChanged.Broadcast(Name);
	return Result;
}

EPropertyBagResult UBlackboardContext::SetValueInt32(const FName Name, const int32 InValue)
{
	PropertyBag.AddProperty(Name, EPropertyBagPropertyType::Int32);
	const EPropertyBagResult Result = PropertyBag.SetValueInt32(Name, InValue);
	OnPropertyChanged.Broadcast(Name);
	return Result;
}

EPropertyBagResult UBlackboardContext::SetValueInt64(const FName Name, const int64 InValue)
{
	PropertyBag.AddProperty(Name, EPropertyBagPropertyType::Int64);
	const EPropertyBagResult Result = PropertyBag.SetValueInt64(Name, InValue);
	OnPropertyChanged.Broadcast(Name);
	return Result;
}

EPropertyBagResult UBlackboardContext::SetValueFloat(const FName Name, const float InValue)
{
	PropertyBag.AddProperty(Name, EPropertyBagPropertyType::Float);
	const EPropertyBagResult Result = PropertyBag.SetValueFloat(Name, InValue);
	OnPropertyChanged.Broadcast(Name);
	return Result;
}

EPropertyBagResult UBlackboardContext::SetValueDouble(const FName Name, const double InValue)
{
	PropertyBag.AddProperty(Name, EPropertyBagPropertyType::Double);
	const EPropertyBagResult Result = PropertyBag.SetValueDouble(Name, InValue);
	OnPropertyChanged.Broadcast(Name);
	return Result;
}

EPropertyBagResult UBlackboardContext::SetValueName(const FName Name, const FName InValue)
{
	PropertyBag.AddProperty(Name, EPropertyBagPropertyType::Name);
	const EPropertyBagResult Result = PropertyBag.SetValueName(Name, InValue);
	OnPropertyChanged.Broadcast(Name);
	return Result;
}

EPropertyBagResult UBlackboardContext::SetValueString(const FName Name, const FString& InValue)
{
	PropertyBag.AddProperty(Name, EPropertyBagPropertyType::String);
	const EPropertyBagResult Result = PropertyBag.SetValueString(Name, InValue);
	OnPropertyChanged.Broadcast(Name);
	return Result;
}

EPropertyBagResult UBlackboardContext::SetValueText(const FName Name, const FText& InValue)
{
	PropertyBag.AddProperty(Name, EPropertyBagPropertyType::Text);
	const EPropertyBagResult Result = PropertyBag.SetValueText(Name, InValue);
	OnPropertyChanged.Broadcast(Name);
	return Result;
}

EPropertyBagResult UBlackboardContext::SetValueStruct(const FName Name, const FInstancedStruct InValue)
{
	PropertyBag.AddProperty(Name, EPropertyBagPropertyType::Struct, InValue.GetScriptStruct());
	const EPropertyBagResult Result = PropertyBag.SetValueStruct(Name, FConstStructView(InValue.GetScriptStruct(), InValue.GetMemory()));
	OnPropertyChanged.Broadcast(Name);
	return Result;
}

EPropertyBagResult UBlackboardContext::SetValueObject(const FName Name, UObject* InValue)
{
	PropertyBag.AddProperty(Name, EPropertyBagPropertyType::Object, InValue);
	const EPropertyBagResult Result = PropertyBag.SetValueObject(Name, InValue);
	OnPropertyChanged.Broadcast(Name);
	return Result;
}

EPropertyBagResult UBlackboardContext::SetValueClass(const FName Name, UClass* InValue)
{
	PropertyBag.AddProperty(Name, EPropertyBagPropertyType::Class, InValue);
	const EPropertyBagResult Result = PropertyBag.SetValueClass(Name, InValue);
	OnPropertyChanged.Broadcast(Name);
	return Result;
}

EPropertyBagResult UBlackboardContext::SetValueSoftPath(const FName Name, const FSoftObjectPath& InValue)
{
	PropertyBag.AddProperty(Name, EPropertyBagPropertyType::SoftClass);
	const EPropertyBagResult Result = PropertyBag.SetValueSoftPath(Name, InValue);
	OnPropertyChanged.Broadcast(Name);
	return Result;
}

EPropertyBagResult UBlackboardContext::SetValueSerializedString(const FName Name, const FString& InValue)
{
	const EPropertyBagResult Result = PropertyBag.SetValueSerializedString(Name, InValue);
	OnPropertyChanged.Broadcast(Name);
	return Result;
}

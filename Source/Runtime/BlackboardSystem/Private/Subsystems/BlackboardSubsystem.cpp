// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Runtime/BlackboardSystem/Public/Subsystems/BlackboardSubsystem.h"
#include "Runtime/BlackboardSystem/Public/Objects/BlackboardContext.h"

void UBlackboardSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GUObjectArray.AddUObjectDeleteListener(this);
	BlackboardContexts.Add(this, NewObject<UBlackboardContext>());
}

void UBlackboardSubsystem::Deinitialize()
{
	OnUObjectArrayShutdown();
	
	Super::Deinitialize();
}

void UBlackboardSubsystem::NotifyUObjectDeleted(const UObjectBase* Object, int32 Index)
{
	BlackboardContexts.Remove(Cast<const UObject>(Object));
}

void UBlackboardSubsystem::OnUObjectArrayShutdown()
{
	GUObjectArray.RemoveUObjectDeleteListener(this);
}

UBlackboardContext* UBlackboardSubsystem::GetBlackboardContext(UObject* Object)
{
	if (!Object)
	{
		return nullptr;
	}
	
	if (UBlackboardContext** Existing = BlackboardContexts.Find(Object))
	{
		return *Existing;
	}
	
	UBlackboardContext* NewContext = NewObject<UBlackboardContext>();
	BlackboardContexts.Add(Object, NewContext);
	return NewContext;
}
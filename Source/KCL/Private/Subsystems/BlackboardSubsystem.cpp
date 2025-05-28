// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Subsystems/BlackboardSubsystem.h"

UBlackboardContext* UBlackboardSubsystem::GetGlobalContext()
{
	if (!GlobalContext)
	{
		GlobalContext = NewObject<UBlackboardContext>(this);
	}
	return GlobalContext;
}

UBlackboardContext* UBlackboardSubsystem::GetActorContext(AActor* Owner)
{
	if (!Owner) return nullptr;
	if (TObjectPtr<UBlackboardContext>* Existing = ActorContexts.Find(Owner))
	{
		return *Existing;
	}
	
	UBlackboardContext* NewContext = NewObject<UBlackboardContext>(Owner);
	ActorContexts.Add(Owner, NewContext);

	Owner->OnDestroyed.AddDynamic(this, &UBlackboardSubsystem::HandleActorDestroyed);
	return NewContext;
}

UBlackboardContext* UBlackboardSubsystem::Static_GetActorContext(AActor* Owner)
{
	if (!Owner) { return nullptr; }
	UGameInstance* GameInstance = Owner->GetGameInstance();
	if (!GameInstance) { return nullptr; }
	UBlackboardSubsystem* BlackboardSubsystem = GameInstance->GetSubsystem<UBlackboardSubsystem>();
	if (!BlackboardSubsystem) { return nullptr; }
	return BlackboardSubsystem->GetActorContext(Owner);
}

void UBlackboardSubsystem::HandleActorDestroyed(AActor* DeadActor)
{
	ActorContexts.Remove(DeadActor);
}

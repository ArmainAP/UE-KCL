// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Subsystems/BlackboardSubsystem.h"

void UBlackboardSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GlobalContext = NewObject<UBlackboardContext>(this);
}

UBlackboardContext* UBlackboardSubsystem::GetGlobalContext() const
{
	return GlobalContext;
}

UBlackboardContext* UBlackboardSubsystem::RegisterActorContext(AActor* Owner)
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

UBlackboardContext* UBlackboardSubsystem::GetActorContext(AActor* Owner) const
{
	if (const TObjectPtr<UBlackboardContext>* Found = ActorContexts.Find(Owner))
	{
		return *Found;
	}
	return nullptr;
}

void UBlackboardSubsystem::HandleActorDestroyed(AActor* DeadActor)
{
	ActorContexts.Remove(DeadActor);
}

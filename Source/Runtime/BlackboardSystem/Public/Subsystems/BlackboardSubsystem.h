// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Objects/BlackboardContext.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BlackboardSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class BLACKBOARDSYSTEM_API UBlackboardSubsystem : public UGameInstanceSubsystem, public FUObjectArray::FUObjectDeleteListener
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void NotifyUObjectDeleted(const UObjectBase* Object, int32 Index) override;
	virtual void OnUObjectArrayShutdown() override;
	
	UFUNCTION(BlueprintCallable)
	UBlackboardContext* GetBlackboardContext(UObject* Object);

private:
	TMap<TWeakObjectPtr<const UObject>, TObjectPtr<UBlackboardContext>> BlackboardContexts;
};
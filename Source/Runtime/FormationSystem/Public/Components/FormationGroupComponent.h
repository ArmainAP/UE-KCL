// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FormationComponent.h"
#include "Components/ActorComponent.h"
#include "Components/ArrowComponent.h"
#include "FormationGroupComponent.generated.h"

class UFormationContext;

USTRUCT(BlueprintType, Blueprintable)
struct FFormationGroupComponentDebug
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDebug = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius=100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Segments=12;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LineColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Thickness = 0.f;
};

UCLASS( BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent), Category="Components|FormationSystem" )
class FORMATIONSYSTEM_API UFormationGroupComponent : public UArrowComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintGetter)
	FString GetID() const { return ID; }

	UFUNCTION(BlueprintGetter)
	UFormationContext* GetContext() const { return Context; }

	UFUNCTION(BlueprintSetter)
	void SetDirection(const FVector& InDirection) { Direction = InDirection; }

	UFUNCTION(BlueprintPure)
	FTransform GetUnitWorldTransform(const int Index) const;

	UFUNCTION(BlueprintPure)
	FTransform GetNextUnitWorldTransform() const;

	UFUNCTION(BlueprintCallable)
	void RequestMove() const;

protected:
	UFUNCTION()
	void OnUnitJoined(UFormationContext* Context, UFormationComponent* Unit);
	
	void OnDebugDraw(UCanvas* Canvas, APlayerController* PlayerController) const;
	
	UPROPERTY(EditAnywhere, BlueprintSetter=SetDirection)
	FVector Direction = FVector::ForwardVector;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UFormationDataAsset> DefaultFormationDataAsset = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter=GetID)
	FString ID;

	UPROPERTY(EditDefaultsOnly)
	bool bAutoDestroyFormation = false;

	UPROPERTY(BlueprintGetter=GetContext)
	TObjectPtr<class UFormationContext> Context = nullptr;

	UPROPERTY(EditAnywhere, Category=Debug)
	FFormationGroupComponentDebug FormationGroupComponentDebug;
	
	FDelegateHandle DebugHandle;
};

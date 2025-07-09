// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TraceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorTraced, const FHitResult&, HitResult);

// Minimal wrapper around the HitResult plus a flag to indicate if it was updated in the current frame
USTRUCT(BlueprintType)
struct FTracedActorInfo
{
    GENERATED_BODY()

    UPROPERTY()
    FHitResult HitResult;

	// The last frame or tick counter in which we updated this actor
	uint64 LastFrameTraced = 0;
};

/**
 * A component that performs a trace in Tick().
 */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(Trace), meta=(BlueprintSpawnableComponent))
class TRACECOMPONENTS_API UTraceComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UTraceComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Trace")
	void GetCurrentHitResults(TArray<FHitResult>& OutArray) const;
	
	UFUNCTION(BlueprintGetter)
	FGameplayTag GetTraceTag() const { return TraceTag; }

	UFUNCTION(BlueprintSetter)
	void SetTraceTag(const FGameplayTag NewTraceTag) { TraceTag = NewTraceTag; };
	
protected:
	UFUNCTION(BlueprintNativeEvent, Category="Trace")
	bool PerformTrace(TArray<FHitResult>& OutHitResults);

	UFUNCTION(BlueprintNativeEvent, Category="Trace")
	void HandleActorTraceBegin(const FHitResult& HitResult);

	UFUNCTION(BlueprintNativeEvent, Category="Trace")
	void HandleActorTraceEnd(const FHitResult& HitResult);

public:
	UPROPERTY(BlueprintAssignable, Category="Trace")
	FActorTraced OnActorTracedBegin;

	UPROPERTY(BlueprintAssignable, Category="Trace")
	FActorTraced OnActorTracedEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trace")
	FVector StartOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trace")
	FVector EndOffset = FVector::OneVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trace")
	bool bTraceComplex = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trace")
	TArray<AActor*> ActorsToIgnore = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trace")
	bool bIgnoreSelf = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trace|Debug")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trace|Debug")
	FLinearColor TraceColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trace|Debug")
	FLinearColor TraceHitColor = FLinearColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trace|Debug")
	float DrawTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Memory")
	int SizePreAllocation = 50;
	
protected:
	// Actor -> traced info
	UPROPERTY()
	TMap<AActor*, FTracedActorInfo> CurrentHitMap;

	// We'll build this array after the trace each frame
	UPROPERTY()
	TArray<FHitResult> CachedHitResults;

	// Used to store the raw trace output each tick
	UPROPERTY()
	TArray<FHitResult> TraceResultsScratch;

	// An incremented counter each tick
	UPROPERTY()
	uint64 CurrentFrameID = 0;

	UPROPERTY(EditAnywhere, BlueprintGetter=GetTraceTag, BlueprintSetter=SetTraceTag)
	FGameplayTag TraceTag;
};
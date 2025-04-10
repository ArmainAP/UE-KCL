// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TraceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorTraced, const FHitResult&, HitResult);

/**
 * A component that performs a trace in Tick().
 */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(Trace), meta=(BlueprintSpawnableComponent))
class TRACECOMPONENTS_API UTraceComponent : public USceneComponent
{
	GENERATED_BODY()

public:

	UTraceComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Trace")
	void GetCurrentHitResults(TArray<FHitResult>& OutArray) const;
	
protected:
	UFUNCTION(BlueprintNativeEvent, Category="Trace")
	bool PerformTrace(TArray<FHitResult>& OutHitResults);

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
	
protected:
	UPROPERTY()
	TMap<AActor*, FHitResult> CurrentHitMap;

	UPROPERTY()
	TSet<AActor*> NewActors;

	UPROPERTY()
	TArray<FHitResult> NewHitResults;
};
// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "TaggedTeamAttitudesSolver.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct KCL_API FQueryAttitudeWrapper
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ETeamAttitude::Type> DefaultSelfAttitude = ETeamAttitude::Friendly;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ETeamAttitude::Type> DefaultOtherAttitude = ETeamAttitude::Neutral;
	
	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<ETeamAttitude::Type>, FGameplayTagQuery> Attitudes;
};

/**
 * 
 */
UCLASS(Config=Engine)
class KCL_API UTaggedTeamAttitudesSolver : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UTaggedTeamAttitudesSolver();

	UFUNCTION(BlueprintPure)
	static FGameplayTag GetTagForTeamId(uint8 TeamID);

	UFUNCTION(BlueprintPure)
	static ETeamAttitude::Type Solver(FGenericTeamId A, FGenericTeamId B);

	UPROPERTY(EditAnywhere)
	FGameplayTag FactionNone;

	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, uint8> FactionTeamIDs;

	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FQueryAttitudeWrapper> FactionAttitudes;
};
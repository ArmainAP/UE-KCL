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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config)
	TEnumAsByte<ETeamAttitude::Type> DefaultSelfAttitude = ETeamAttitude::Friendly;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config)
	TEnumAsByte<ETeamAttitude::Type> DefaultOtherAttitude = ETeamAttitude::Neutral;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config)
	TMap<TEnumAsByte<ETeamAttitude::Type>, FGameplayTagQuery> Attitudes;
};

/**
 * 
 */
UCLASS(Config=Engine, DefaultConfig)
class KCL_API UTaggedTeamAttitudesSolver : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UTaggedTeamAttitudesSolver();

	UFUNCTION(BlueprintPure)
	static uint8 GetTeamIdForTag(FGameplayTag TeamTag);
	
	UFUNCTION(BlueprintPure)
	static FGameplayTag GetTagForTeamId(uint8 TeamID);

	UFUNCTION(BlueprintPure)
	static ETeamAttitude::Type Solver(FGenericTeamId A, FGenericTeamId B);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config)
	FGameplayTag FactionNone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config)
	TMap<uint8, FGameplayTag> FactionTeamIDs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config)
	TMap<FGameplayTag, FQueryAttitudeWrapper> FactionAttitudes;
};
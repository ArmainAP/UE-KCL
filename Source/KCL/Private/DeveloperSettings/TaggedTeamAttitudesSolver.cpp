// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "DeveloperSettings/TaggedTeamAttitudesSolver.h"
#include "GenericTeamAgentInterface.h"

UTaggedTeamAttitudesSolver::UTaggedTeamAttitudesSolver()
{
	CategoryName = "KCL";
	SectionName = "Tagged Team Attitudes Solver";
}

FGameplayTag UTaggedTeamAttitudesSolver::GetTagForTeamId(uint8 TeamID)
{
	const UTaggedTeamAttitudesSolver* Settings = GetDefault<UTaggedTeamAttitudesSolver>();
	if (const FGameplayTag* Found = Settings->FactionTeamIDs.FindKey(TeamID))
	{
		return *Found;
	}
	return Settings->FactionNone;
}

ETeamAttitude::Type UTaggedTeamAttitudesSolver::Solver(FGenericTeamId A, FGenericTeamId B)
{
	const uint8 IdA = A.GetId();
	const uint8 IdB = B.GetId();

	const FGameplayTag TagA = GetTagForTeamId(IdA);
	const FGameplayTag TagB = GetTagForTeamId(IdB);

	// Build a minimal container for the “other” actor’s tags
	FGameplayTagContainer OtherTags;
	OtherTags.AddTag(TagB);

	// Look up your wrapper for TagA
	const UTaggedTeamAttitudesSolver* Settings = GetDefault<UTaggedTeamAttitudesSolver>();
	if (const FQueryAttitudeWrapper* Wrapper = Settings->FactionAttitudes.Find(TagA))
	{
		// Try each attitude in the order designers placed them
		for (const auto& Pair : Wrapper->Attitudes)
		{
			const ETeamAttitude::Type   Att = Pair.Key;
			const FGameplayTagQuery& Query = Pair.Value;

			if (Query.Matches(OtherTags))
			{
				return Att;
			}
		}

		return (IdA == IdB) ? Wrapper->DefaultSelfAttitude : Wrapper->DefaultOtherAttitude; 
	}

	// Default if nothing matched
	return ETeamAttitude::Neutral;
}
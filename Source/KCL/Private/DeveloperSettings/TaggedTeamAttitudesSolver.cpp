// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "DeveloperSettings/TaggedTeamAttitudesSolver.h"

static TMap<FGameplayTag, uint8> CachedReverseLookup;

UTaggedTeamAttitudesSolver::UTaggedTeamAttitudesSolver()
{
	CategoryName = "KCL";
	SectionName = "Tagged Team Attitudes Solver";
}

uint8 UTaggedTeamAttitudesSolver::GetTeamIdForTag(FGameplayTag TeamTag)
{
	if (const uint8* Id = CachedReverseLookup.Find(TeamTag))
	{
		return *Id;
	}

	const UTaggedTeamAttitudesSolver* Settings = GetDefault<UTaggedTeamAttitudesSolver>();
	for (FGameplayTag Current = TeamTag; Current.IsValid(); Current = Current.RequestDirectParent())
	{
		if (const uint8* Id = Settings->FactionTeamIDs.FindKey(Current))
		{
			for (FGameplayTag T = TeamTag; T != Current; T = T.RequestDirectParent())
			{
				CachedReverseLookup.Add(T, *Id);
			}
			CachedReverseLookup.Add(Current, *Id);
			return *Id;
		}
	}

	return FGenericTeamId::NoTeam;
}

FGameplayTag UTaggedTeamAttitudesSolver::GetTagForTeamId(uint8 TeamID)
{
	const UTaggedTeamAttitudesSolver* Settings = GetDefault<UTaggedTeamAttitudesSolver>();
	if (const FGameplayTag* Found = Settings->FactionTeamIDs.Find(TeamID))
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
			const ETeamAttitude::Type Att = Pair.Key;
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
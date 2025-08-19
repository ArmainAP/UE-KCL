// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "SpawnHandlers/FormationSpawnHandler.h"

#include "Runtime/FormationSystem/Public/Data/FormationDataAssets/FormationDataAsset.h"

void UFormationSpawnHandler::BeginSpawnActor_Implementation()
{
	const FTransform SpawnTransform = GetSpawnActorTransform(); 
	TArray<FTransform> Transforms;
	FormationDataAsset->GetOffsetTransforms(SpawnCount, Transforms);
	for (int Index = 0; Index < SpawnCount; Index++)
	{
		SpawnActor(Transforms[Index] * SpawnTransform);
	}
}

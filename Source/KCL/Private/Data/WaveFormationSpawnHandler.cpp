// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Data/WaveFormationSpawnHandler.h"

#include "Data/BatchSpawnData.h"
#include "Runtime/FormationSystem/Public/Data/FormationDataAssets/FormationDataAsset.h"
#include "SpawnPoint/WaveSpawnPoint.h"

void UWaveFormationSpawnHandler::BeginSpawn_Implementation()
{
	int DesiredCount = FormationCount;
	if (const int Delta = BatchSpawnData.SpawnCount - SpawnedCount; Delta < FormationCount)
	{
		DesiredCount = Delta;
	}
	
	TArray<FTransform> Transforms;
	FormationDataAsset->GetWorldTransforms(DesiredCount, SpawnPoint->GetActorLocation(), SpawnPoint->GetActorForwardVector(), Transforms);
	for (int Index = 0; Index < DesiredCount; Index++)
	{
		SpawnActor(Transforms[Index]);
	}
}

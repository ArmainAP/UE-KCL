// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Data/WaveFormationSpawnHandler.h"
#include "Data/FormationDataAsset.h"

void UWaveFormationSpawnHandler::OnSpawnActor_Implementation()
{
	if (!IsValid(SpawnPoint) || !BatchSpawnData.SpawnedActor || !IsValid(FormationDataAsset))
	{
		CancelSpawn();
		return;
	}

	if (SpawnedCount >= BatchSpawnData.SpawnCount)
	{
		CancelSpawn();
		OnBatchComplete.Broadcast();
		return;
	}

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

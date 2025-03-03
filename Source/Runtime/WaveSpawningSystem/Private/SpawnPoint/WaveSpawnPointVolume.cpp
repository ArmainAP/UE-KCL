// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "SpawnPoint/WaveSpawnPointVolume.h"

#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

AWaveSpawnPointVolume::AWaveSpawnPointVolume()
{
	SpawnVolumeBox = CreateDefaultSubobject<UBoxComponent>("SpawnVolumeBox");
	SpawnVolumeBox->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	SpawnVolumeBox->SetupAttachment(RootComponent);
}

FTransform AWaveSpawnPointVolume::GetSpawnPointTransform()
{
	FTransform Transform = Super::GetSpawnPointTransform();
	FVector Origin, Extent; float Radius;
	UKismetSystemLibrary::GetComponentBounds(SpawnVolumeBox, Origin, Extent, Radius);
	Transform.SetLocation(UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent));
	return Transform;
}

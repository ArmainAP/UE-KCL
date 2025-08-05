// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Actors/WorldActorSpawnerVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

AWorldActorSpawnerVolume::AWorldActorSpawnerVolume(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SpawnVolumeBox = CreateDefaultSubobject<UBoxComponent>(GET_MEMBER_NAME_CHECKED(AWorldActorSpawnerVolume, SpawnVolumeBox));
	SpawnVolumeBox->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	SpawnVolumeBox->SetBoxExtent(FVector(100, 100, 100));
	SpawnVolumeBox->SetupAttachment(RootComponent);
}

FTransform AWorldActorSpawnerVolume::GetSpawnActorTransform_Implementation() const
{
	FVector Origin, Extent; float Radius;
	UKismetSystemLibrary::GetComponentBounds(SpawnVolumeBox, Origin, Extent, Radius);

	FTransform Transform = GetActorTransform();
	Transform.SetLocation(UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent));
	return Transform;
}

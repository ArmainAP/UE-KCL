// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Data/SightSystemMultipliers.h"
#include "SightedComponent.generated.h"


UCLASS(Blueprintable, BlueprintType, ClassGroup=(SightSystem), meta=(BlueprintSpawnableComponent) )
class SIGHTSYSTEM_API USightedComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USightedComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	float GainRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight System")
	FSightSystemMultipliers SightSystemMultipliers;

protected:
	virtual void BeginPlay() override;
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;
};

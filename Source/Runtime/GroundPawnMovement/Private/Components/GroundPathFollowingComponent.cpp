// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/GroundPathFollowingComponent.h"

#include "AIController.h"
#include "Misc/DataValidation.h"

#if WITH_EDITOR
EDataValidationResult UGroundPathFollowingComponent::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!Cast<AAIController>(GetOwner()))
	{
		const FString Error = FString::Printf(TEXT("%s is not a child of %s"), *GetOwner()->GetName(), *AAIController::StaticClass()->GetName());
		Context.AddError(FText::FromString(Error));
		Result = EDataValidationResult::Invalid;
	}
	return Result;
}
#endif
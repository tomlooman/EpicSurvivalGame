// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SCharacterMovementComponent.h"
#include "SBaseCharacter.h"



float USCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	const ASBaseCharacter* CharOwner = Cast<ASBaseCharacter>(PawnOwner);
	if (CharOwner)
	{
		// Slow down during targeting or crouching
		if (CharOwner->IsTargeting() && !IsCrouching())
		{
			MaxSpeed *= CharOwner->GetTargetingSpeedModifier();
		}
		else if (CharOwner->IsSprinting())
		{
			MaxSpeed *= CharOwner->GetSprintingSpeedModifier();
		}
	}

	return MaxSpeed;
}
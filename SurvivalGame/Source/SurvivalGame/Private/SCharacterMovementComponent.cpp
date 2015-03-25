// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SCharacterMovementComponent.h"
#include "SCharacter.h"



float USCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	const ASCharacter* CharOwner = Cast<ASCharacter>(PawnOwner);
	if (CharOwner)
	{
		// Slow down during targeting
		if (CharOwner->IsTargeting())
		{
			MaxSpeed *= CharOwner->GetTargetingSpeedModifier();
		}
	}

	return MaxSpeed;
}
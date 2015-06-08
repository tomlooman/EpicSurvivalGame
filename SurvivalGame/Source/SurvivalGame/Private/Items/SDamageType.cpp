// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SDamageType.h"



USDamageType::USDamageType(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	/* We apply this modifier based on the physics material setup to the head of the enemy PhysAsset */
	HeadDmgModifier = 2.0f;
	LimbDmgModifier = 0.5f;

	bCanDieFrom = true;
}


bool USDamageType::GetCanDieFrom()
{
	return bCanDieFrom;
}


float USDamageType::GetHeadDamageModifier()
{
	return HeadDmgModifier;
}

float USDamageType::GetLimbDamageModifier()
{
	return LimbDmgModifier;
}

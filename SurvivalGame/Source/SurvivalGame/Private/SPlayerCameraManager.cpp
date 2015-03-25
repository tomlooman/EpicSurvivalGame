// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SPlayerCameraManager.h"
#include "SCharacter.h"


ASPlayerCameraManager::ASPlayerCameraManager(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NormalFOV = 90.0f;
	TargetingFOV = 65.0f;

	ViewPitchMin = -80.0f;
	ViewPitchMax = 87.0f;
	bAlwaysApplyModifiers = true;
}


void ASPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	ASCharacter* MyPawn = PCOwner ? Cast<ASCharacter>(PCOwner->GetPawn()) : NULL;
	if (MyPawn)
	{
		const float TargetFOV = MyPawn->IsTargeting() ? TargetingFOV : NormalFOV;
		DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetFOV, DeltaTime, 20.0f);
		SetFOV(DefaultFOV);
	}

	Super::UpdateCamera(DeltaTime);
}
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "SPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API ASPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_UCLASS_BODY()

	/* Update the FOV */
	virtual void UpdateCamera(float DeltaTime) override;

public:

	/* default, hip fire FOV */
	float NormalFOV;

	/* aiming down sight / zoomed FOV */
	float TargetingFOV;
};

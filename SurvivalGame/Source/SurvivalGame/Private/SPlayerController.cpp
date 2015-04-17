// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SPlayerController.h"
#include "SPlayerCameraManager.h"


ASPlayerController::ASPlayerController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = ASPlayerCameraManager::StaticClass();
}


void ASPlayerController::UnFreeze()
{
	/* Respawn */
	ServerRestartPlayer();
}

// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#include "World/SOpenWorldGameMode.h"





ASOpenWorldGameMode::ASOpenWorldGameMode()
{
	/* Open world content is free-for-all */
	bAllowFriendlyFireDamage = true;
}


bool ASOpenWorldGameMode::CanSpectate_Implementation(APlayerController* Viewer, APlayerState* ViewTarget)
{
	return false;
}

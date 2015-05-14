// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SLocalPlayer.h"






FString USLocalPlayer::GetNickname() const
{
	/* Try to fetch a nickname from the online subsystem (eg. Steam) if available */
	FString NickName = Super::GetNickname();

	if (NickName.IsEmpty())
	{
		// Fall back if no nickname was available through the online subsystem.
		NickName = FWindowsPlatformProcess::ComputerName() + FString::FromInt(FMath::RandRange(0, 999));
	}

	return NickName;
}

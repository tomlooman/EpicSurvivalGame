// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "SHUD.h"
#include "SPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API ASPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

	/* Flag to respawn or start spectating upon death */
	bool bRespawnImmediately;
	
	/* Respawn or start spectating after dying */
	virtual void UnFreeze() override;

	/* Start spectating. Should be called only on server */
	void StartSpectating();

	UFUNCTION(reliable, server, WithValidation)
	void ServerSuicide();

	UFUNCTION(reliable, client)
	void ClientHUDStateChanged(EHUDState NewState);

public:

	/* Kill the current pawn */
	UFUNCTION(exec)
	virtual void Suicide();

	/* End spectating and respawn player. Should be called only on server */
	void RespawnPlayer();
};

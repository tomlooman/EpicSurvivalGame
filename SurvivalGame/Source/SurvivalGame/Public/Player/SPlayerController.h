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
	GENERATED_BODY()

	ASPlayerController(const FObjectInitializer& ObjectInitializer);

	/* Flag to respawn or start spectating upon death */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	bool bRespawnImmediately;
	
	/* Respawn or start spectating after dying */
	virtual void UnFreeze() override;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSuicide();

	void ServerSuicide_Implementation();

	bool ServerSuicide_Validate();

public:

	UFUNCTION(Reliable, Client)
	void ClientHUDStateChanged(EHUDState NewState);

	void ClientHUDStateChanged_Implementation(EHUDState NewState);

	UFUNCTION(Reliable, Client)
	void ClientMessageReceived(const FString& TextMessage);

	void ClientMessageReceived_Implementation(const FString& TextMessage);

	/* Kill the current pawn */
	UFUNCTION(exec)
	virtual void Suicide();

	/* Start spectating. Should be called only on server */
	void StartSpectating();
};

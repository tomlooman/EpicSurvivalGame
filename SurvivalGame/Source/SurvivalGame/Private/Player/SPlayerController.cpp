// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SPlayerController.h"
#include "SPlayerCameraManager.h"
#include "SCharacter.h"
#include "STypes.h"
#include "SHUD.h"
#include "SGameState.h"


ASPlayerController::ASPlayerController(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	/* Assign the class types we wish to use */
	PlayerCameraManagerClass = ASPlayerCameraManager::StaticClass();

	/* Example - Can be set to true for debugging, generally a value like this would exist in the GameMode instead */
	bRespawnImmediately = false;
}


void ASPlayerController::UnFreeze()
{
	Super::UnFreeze();

	// Check if match is ending or has ended.
	ASGameState* MyGameState = Cast<ASGameState>(GetWorld()->GameState);
	if (MyGameState && MyGameState->HasMatchEnded())
	{
		/* Don't allow spectating or respawns */
		return;
	}

	/* Respawn or spectate */
	if (bRespawnImmediately)
	{
		ServerRestartPlayer();
	}
	else
	{
		StartSpectating();
	}
}


void ASPlayerController::StartSpectating()
{
	/* Update the state on server */
	PlayerState->bIsSpectator = true;
	/* Waiting to respawn */
	bPlayerIsWaiting = true;
	ChangeState(NAME_Spectating);
	/* Push the state update to the client */
	ClientGotoState(NAME_Spectating);

	/* Focus on the remaining alive player */
	ViewAPlayer(1);

	/* Update the HUD to show the spectator screen */
	ClientHUDStateChanged(EHUDState::Spectating);
}


void ASPlayerController::Suicide()
{
	if (IsInState(NAME_Playing))
	{
		ServerSuicide();
	}
}

void ASPlayerController::ServerSuicide_Implementation()
{
	ASCharacter* MyPawn = Cast<ASCharacter>(GetPawn());
	if (MyPawn && ((GetWorld()->TimeSeconds - MyPawn->CreationTime > 1) || (GetNetMode() == NM_Standalone)))
	{
		MyPawn->Suicide();
	}
}


bool ASPlayerController::ServerSuicide_Validate()
{
	return true;
}


void ASPlayerController::ClientHUDStateChanged_Implementation(EHUDState NewState)
{
	ASHUD* MyHUD = Cast<ASHUD>(GetHUD());
	if (MyHUD)
	{
		MyHUD->OnStateChanged(NewState);
	}
}


void ASPlayerController::ClientMessageReceived_Implementation(const FString& TextMessage)
{
	ASHUD* MyHUD = Cast<ASHUD>(GetHUD());
	if (MyHUD)
	{
		MyHUD->MessageReceived(TextMessage);
	}
}

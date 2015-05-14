// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SPlayerController.h"
#include "SPlayerCameraManager.h"
#include "SCharacter.h"
#include "STypes.h"
#include "SHUD.h"


ASPlayerController::ASPlayerController(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	/* Assign the class types we wish to use */
	PlayerCameraManagerClass = ASPlayerCameraManager::StaticClass();

	bRespawnImmediately = false;
}


void ASPlayerController::UnFreeze()
{
	Super::UnFreeze();

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
	PlayerState->bIsSpectator = true;
	ChangeState(NAME_Spectating);
	ClientGotoState(NAME_Spectating);

	ClientHUDStateChanged(EHUDState::Spectating);
}


void ASPlayerController::RespawnPlayer()
{
	bPlayerIsWaiting = true;
	ServerRestartPlayer();

	ClientHUDStateChanged(EHUDState::Playing);
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

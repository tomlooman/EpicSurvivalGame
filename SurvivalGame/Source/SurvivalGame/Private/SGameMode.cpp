// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SGameMode.h"
#include "SPlayerController.h"
#include "SPlayerState.h"
#include "SGameState.h"


ASGameMode::ASGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/* Assign the class types used by this gamemode */
	PlayerControllerClass = ASPlayerController::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();
	GameStateClass = ASGameState::StaticClass();
	
	// TODO: Assign PlayerPawn through objectfinder trick, but explain how this should generally be avoided.

	bAllowFriendlyFireDamage = false;
}


bool ASGameMode::CanDealDamage(class ASPlayerState* DamageCauser, class ASPlayerState* DamagedPlayer) const
{
	if (bAllowFriendlyFireDamage)
		return true;

	// TODO: Compare Team Numbers;
	return DamageCauser && DamagedPlayer && (DamageCauser->GetTeamNumber() != DamagedPlayer->GetTeamNumber());
}


FString ASGameMode::InitNewPlayer(class APlayerController* NewPlayerController, const TSharedPtr<FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal)
{
	FString Result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	// TODO: Find bot vs. player type and assign a different team to bots.

	ASPlayerState* NewPlayerState = Cast<ASPlayerState>(NewPlayerController->PlayerState);
	if (NewPlayerState)
	{
		NewPlayerState->SetTeamNumber(PlayerTeamNum);
	}

	return Result;
}

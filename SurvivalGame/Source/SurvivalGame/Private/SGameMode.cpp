// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SGameMode.h"
#include "SPlayerController.h"
#include "SPlayerState.h"
#include "SGameState.h"
#include "SCharacter.h"
#include "SHUD.h"


ASGameMode::ASGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/* Assign the class types used by this gamemode */
	PlayerControllerClass = ASPlayerController::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();
	GameStateClass = ASGameState::StaticClass();	

	bAllowFriendlyFireDamage = false;

	/* Default team is 0 */
	PlayerTeamNum = 1;
}


void ASGameMode::DefaultTimer()
{
	/* This function is called every 1 second. */
	Super::DefaultTimer();

	ASGameState* MyGameState = Cast<ASGameState>(GameState);
	if (MyGameState)
	{
		/* Increment our time of day */
		MyGameState->ElapsedGameMinutes += MyGameState->GetTimeOfDayIncrement();

		const float TimeOfDay = MyGameState->ElapsedGameMinutes - MyGameState->GetElapsedDaysInMinutes();
		if (TimeOfDay > (6 * 60) && TimeOfDay < (18 * 60))
		{
			MyGameState->bIsNight = false;
		}
		else
		{
			MyGameState->bIsNight = true;
		}

		if (MyGameState->bIsNight != bWasNight)
		{
			FString MessageText = MyGameState->bIsNight ? "NIGHT HAS FALLEN" : "SUNRISE!";

			// Warn ALL alive players that we entered day or night through their HUD instances.
			for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
			{
				ASCharacter* PlayerPawn = Cast<ASCharacter>(*It);
				if (PlayerPawn)
				{
					ASPlayerController* MyController = Cast<ASPlayerController>(PlayerPawn->GetController());
					if (MyController)
					{
						ASHUD* MyHUD = Cast<ASHUD>(MyController->GetHUD());
						if (MyHUD)
						{
							MyHUD->MessageReceived(MessageText);
						}
					}
				}
			}
		}

		bWasNight = MyGameState->bIsNight;
	}
}


bool ASGameMode::CanDealDamage(class ASPlayerState* DamageCauser, class ASPlayerState* DamagedPlayer) const
{
	if (bAllowFriendlyFireDamage)
		return true;

	// Compare Team Numbers
	return DamageCauser && DamagedPlayer && (DamageCauser->GetTeamNumber() != DamagedPlayer->GetTeamNumber());
}


FString ASGameMode::InitNewPlayer(class APlayerController* NewPlayerController, const TSharedPtr<FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal)
{
	FString Result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	ASPlayerState* NewPlayerState = Cast<ASPlayerState>(NewPlayerController->PlayerState);
	if (NewPlayerState)
	{
		NewPlayerState->SetTeamNumber(PlayerTeamNum);
	}

	return Result;
}


float ASGameMode::ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	float ActualDamage = Damage;

	ASBaseCharacter* DamagedPawn = Cast<ASBaseCharacter>(DamagedActor);
	if (DamagedPawn && EventInstigator)
	{
		ASPlayerState* DamagedPlayerState = Cast<ASPlayerState>(DamagedPawn->PlayerState);
		ASPlayerState* InstigatorPlayerState = Cast<ASPlayerState>(EventInstigator->PlayerState);

		// Check for friendly fire
		if (!CanDealDamage(InstigatorPlayerState, DamagedPlayerState))
		{
			ActualDamage = 0.f;
		}
	}

	return ActualDamage;
}
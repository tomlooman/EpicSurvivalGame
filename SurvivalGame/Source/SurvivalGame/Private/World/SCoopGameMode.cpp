// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SCoopGameMode.h"



/* Define a log category for error messages */
DEFINE_LOG_CATEGORY_STATIC(LogGameMode, Log, All);



ASCoopGameMode::ASCoopGameMode(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	/* Disable damage to coop buddies  */
	bAllowFriendlyFireDamage = false;
	bSpawnAtTeamPlayer = true;
	
	ScoreNightSurvived = 1000;
}


/*
	RestartPlayer - Spawn the player next to his living coop buddy instead of a PlayerStart
*/
void ASCoopGameMode::RestartPlayer(class AController* NewPlayer)
{
	/* Fallback to PlayerStart picking if team spawning is disabled or we're trying to spawn a bot. */
	if (!bSpawnAtTeamPlayer || (NewPlayer->PlayerState && NewPlayer->PlayerState->bIsABot))
	{
		Super::RestartPlayer(NewPlayer);
		return;
	}

	/* Look for a live player to spawn next to */
	FVector SpawnOrigin = FVector::ZeroVector;
	FRotator StartRotation = FRotator::ZeroRotator;
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
	{
		ASCharacter* MyCharacter = Cast<ASCharacter>(*It);
		if (MyCharacter && MyCharacter->IsAlive())
		{
			/* Get the origin of the first player we can find */
			SpawnOrigin = MyCharacter->GetActorLocation();
			StartRotation = MyCharacter->GetActorRotation();
			break;
		}
	}

	/* No player is alive (yet) - spawn using one of the PlayerStarts */
	if (SpawnOrigin == FVector::ZeroVector)
	{
		Super::RestartPlayer(NewPlayer);
		return;
	}

	/* Get a point on the nav mesh near the other player */
	FVector StartLocation = UNavigationSystem::GetRandomPointInNavigableRadius(NewPlayer, SpawnOrigin, 250.0f);

	// Try to create a pawn to use of the default class for this player
	if (NewPlayer->GetPawn() == nullptr && GetDefaultPawnClassForController(NewPlayer) != nullptr)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Instigator = Instigator;
		APawn* ResultPawn = GetWorld()->SpawnActor<APawn>(GetDefaultPawnClassForController(NewPlayer), StartLocation, StartRotation, SpawnInfo);
		if (ResultPawn == nullptr)
		{
			UE_LOG(LogGameMode, Warning, TEXT("Couldn't spawn Pawn of type %s at %s"), *GetNameSafe(DefaultPawnClass), &StartLocation);
		}
		NewPlayer->SetPawn(ResultPawn);
	}

	if (NewPlayer->GetPawn() == nullptr)
	{
		NewPlayer->FailedToSpawnPawn();
	}
	else
	{
		NewPlayer->Possess(NewPlayer->GetPawn());

		// If the Pawn is destroyed as part of possession we have to abort
		if (NewPlayer->GetPawn() == nullptr)
		{
			NewPlayer->FailedToSpawnPawn();
		}
		else
		{
			// Set initial control rotation to player start's rotation
			NewPlayer->ClientSetRotation(NewPlayer->GetPawn()->GetActorRotation(), true);

			FRotator NewControllerRot = StartRotation;
			NewControllerRot.Roll = 0.f;
			NewPlayer->SetControlRotation(NewControllerRot);

			SetPlayerDefaults(NewPlayer->GetPawn());
		}
	}
}


void ASCoopGameMode::OnNightEnded()
{
	/* Respawn spectating players that died during the night */
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		/* Look for all players that are spectating */
		ASPlayerController* MyController = Cast<ASPlayerController>(*It);
		if (MyController)
		{
			if (MyController->PlayerState->bIsSpectator)
			{
				RestartPlayer(MyController);
				MyController->ClientHUDStateChanged(EHUDState::Playing);
			}
			else
			{
				/* Player still alive, award him some points */
				ASCharacter* MyPawn = Cast<ASCharacter>(MyController->GetPawn());
				if (MyPawn && MyPawn->IsAlive())
				{
					ASPlayerState* PS = Cast<ASPlayerState>(MyController->PlayerState);
					if (PS)
					{
						PS->ScorePoints(ScoreNightSurvived);
					}
				}
			}
		}
	}
}


void ASCoopGameMode::Killed(AController* Killer, AController* VictimPlayer, APawn* VictimPawn, const UDamageType* DamageType)
{
	ASPlayerState* KillerPS = Killer ? Cast<ASPlayerState>(Killer->PlayerState) : NULL;
	ASPlayerState* VictimPS = VictimPlayer ? Cast<ASPlayerState>(VictimPlayer->PlayerState) : NULL;

	if (KillerPS && KillerPS != VictimPS && !KillerPS->bIsABot)
	{
		KillerPS->AddKill();
		KillerPS->ScorePoints(10);
	}

	if (VictimPS && !VictimPS->bIsABot)
	{
		VictimPS->AddDeath();
	}

	/* End match is all players died */
	CheckMatchEnd();
}


void ASCoopGameMode::CheckMatchEnd()
{
	bool bHasAlivePlayer = false;
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
	{
		ASCharacter* MyPawn = Cast<ASCharacter>(*It);
		if (MyPawn && MyPawn->IsAlive())
		{
			ASPlayerState* PS = Cast<ASPlayerState>(MyPawn->PlayerState);
			if (PS)
			{
				if (!PS->bIsABot)
				{
					/* Found one player that is still alive, game will continue */
					bHasAlivePlayer = true;
					break;
				}
			}
		}
	}

	/* End game is all players died */
	if (!bHasAlivePlayer)
	{
		FinishMatch();
	}
}


void ASCoopGameMode::FinishMatch()
{
	ASGameState* const MyGameState = Cast<ASGameState>(GameState);
	if (IsMatchInProgress())
	{
		EndMatch();

		/* Stop spawning bots */
		GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawns);

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
		{
			ASPlayerController* MyController = Cast<ASPlayerController>(*It);
			if (MyController)
			{
				MyController->ClientHUDStateChanged(EHUDState::MatchEnd);
			}
		}
	}
}

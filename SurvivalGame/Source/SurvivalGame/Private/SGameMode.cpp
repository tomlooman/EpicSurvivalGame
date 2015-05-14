// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SGameMode.h"
#include "SPlayerController.h"
#include "SPlayerState.h"
#include "SGameState.h"
#include "SCharacter.h"
#include "SHUD.h"
#include "STypes.h"
#include "SSpectatorPawn.h"
#include "SZombieAIController.h"


ASGameMode::ASGameMode(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	/* Assign the class types used by this gamemode */
	PlayerControllerClass = ASPlayerController::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();
	GameStateClass = ASGameState::StaticClass();
	SpectatorClass = ASSpectatorPawn::StaticClass();

	bAllowFriendlyFireDamage = false;

	/* Default team is 1 for players and 0 for enemies */
	PlayerTeamNum = 1;
}


void ASGameMode::DefaultTimer()
{
	/* This function is called every 1 second. */
	Super::DefaultTimer();

	/* Immediately start the match while playing in editor */
	//if (GetWorld()->IsPlayInEditor())
	{
		if (GetMatchState() == MatchState::WaitingToStart)
		{
			StartMatch();
		}
	}

	/* Only increment time of day while game is active */
	if (IsMatchInProgress())
	{
		ASGameState* MyGameState = Cast<ASGameState>(GameState);
		if (MyGameState)
		{
			/* Increment our time of day */
			MyGameState->ElapsedGameMinutes += MyGameState->GetTimeOfDayIncrement();

			/* Determine our state */
			const float TimeOfDay = MyGameState->ElapsedGameMinutes - MyGameState->GetElapsedDaysInMinutes();
			if (TimeOfDay > (6 * 60) && TimeOfDay < (18 * 60))
			{
				MyGameState->bIsNight = false;
			}
			else
			{
				MyGameState->bIsNight = true;
			}

			/* Trigger events when night starts or ends */
			if (MyGameState->bIsNight != bWasNight)
			{
				FString MessageText = MyGameState->bIsNight ? "NIGHT HAS FALLEN" : "SUNRISE!";

				ASGameState* MyGameState = Cast<ASGameState>(GameState);
				if (MyGameState)
				{
					MyGameState->BroadcastGameMessage(MessageText);
				}

				/* The night just ended, respawn all dead players */
				if (!MyGameState->bIsNight)
				{
					/* Respawn spectating players that died during the night */
					for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
					{
						/* Look for all players that are spectating */
						ASPlayerController* MyController = Cast<ASPlayerController>(*It);
						if (MyController && MyController->PlayerState->bIsSpectator)
						{
							MyController->RespawnPlayer();
						}
					}
				}
			}

			if (MyGameState->bIsNight)
			{
				// Spawn a new enemy. TEMPORARY ONLY
				SpawnNewBot();
			}

			bWasNight = MyGameState->bIsNight;
		}
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


void ASGameMode::CheckMatchEnd()
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


void ASGameMode::FinishMatch()
{
	ASGameState* const MyGameState = Cast<ASGameState>(GameState);
	if (IsMatchInProgress())
	{
		EndMatch();

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

void ASGameMode::Killed(AController* Killer, AController* VictimPlayer, APawn* VictimPawn, const UDamageType* DamageType)
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

		// TODO: Let coop buddy know this player died
		//VictimPS->BroadcastDeath(KillerPS, DamageType, VictimPS);
	}

	/* End match is all players died */
	CheckMatchEnd();
}


bool ASGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	/* Always pick a random location */
	return false;
}


AActor* ASGameMode::ChoosePlayerStart(AController* Player)
{
	TArray<APlayerStart*> PreferredSpawns;
	TArray<APlayerStart*> FallbackSpawns;

	for (int32 i = 0; i < PlayerStarts.Num(); i++)
	{
		APlayerStart* TestStart = PlayerStarts[i];
		if (IsSpawnpointAllowed(TestStart, Player))
		{
			if (IsSpawnpointPreferred(TestStart, Player))
			{
				PreferredSpawns.Add(TestStart);
			}
			else
			{
				FallbackSpawns.Add(TestStart);
			}
		}
	}

	APlayerStart* BestStart = nullptr;
	if (PreferredSpawns.Num() > 0)
	{
		BestStart = PreferredSpawns[FMath::RandHelper(PreferredSpawns.Num())];
	}
	else if (FallbackSpawns.Num() > 0)
	{
		BestStart = FallbackSpawns[FMath::RandHelper(FallbackSpawns.Num())];
	}

	return BestStart ? BestStart : Super::ChoosePlayerStart(Player);
}


bool ASGameMode::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Controller)
{
	/* No tag was specified, we will treat it as anyone may spawn here  */
	if (SpawnPoint->PlayerStartTag.IsNone())
		return true;

	if (Controller == nullptr || Controller->PlayerState == nullptr)
		return true;

	if (Controller->PlayerState->bIsABot && SpawnPoint->PlayerStartTag.IsEqual(NAME_EnemyStart))
	{
		return true;
	}
	else if (!Controller->PlayerState->bIsABot && SpawnPoint->PlayerStartTag.IsEqual(NAME_PlayerStart))
	{
		return true;
	}

	return false;
}


bool ASGameMode::IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Controller)
{
	if (SpawnPoint)
	{
		const FVector SpawnLocation = SpawnPoint->GetActorLocation();
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
		{
			ACharacter* OtherPawn = Cast<ACharacter>(*It);
			if (OtherPawn)
			{
				const float CombinedHeight = (SpawnPoint->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) * 2.0f;
				const float CombinedWidth = SpawnPoint->GetCapsuleComponent()->GetScaledCapsuleRadius() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleRadius();
				const FVector OtherLocation = OtherPawn->GetActorLocation();

				// Check if player overlaps the playerstart
				if (FMath::Abs(SpawnLocation.Z - OtherLocation.Z) < CombinedHeight && (SpawnLocation - OtherLocation).Size2D() < CombinedWidth)
				{
					return false;
				}
			}
		}
	}

	return true;
}


void ASGameMode::SpawnNewBot()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail = true;

	ASZombieAIController* AIC = GetWorld()->SpawnActor<ASZombieAIController>(SpawnInfo);
	RestartPlayer(AIC);
}

/* Used by RestartPlayer() to determine the pawn to create and possess when a bot or player spawns */
UClass* ASGameMode::GetDefaultPawnClassForController(AController* InController)
{
	if (Cast<ASZombieAIController>(InController))
	{
		return BotPawnClass;
	}

	return Super::GetDefaultPawnClassForController(InController);
}

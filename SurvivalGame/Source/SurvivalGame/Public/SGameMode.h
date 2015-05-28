// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameMode.h"
#include "SGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API ASGameMode : public AGameMode
{
	GENERATED_BODY()

	ASGameMode(const FObjectInitializer& ObjectInitializer);

	virtual void InitGameState();

	virtual void DefaultTimer() override;
	
	virtual void StartMatch();

	/* End the match when all players are dead */
	void CheckMatchEnd();

	/* End the match, with a delay before returning to the main menu */
	void FinishMatch();

	/* Can we deal damage to players in the same team */
	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	bool bAllowFriendlyFireDamage;

	/* Spawn at team player if any are alive */
	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	bool bSpawnAtTeamPlayer;

	/* Allow zombie spawns to be disabled (for debugging) */
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool bSpawnZombiesAtNight;

	/* Called once on every new player that enters the gamemode */
	virtual FString InitNewPlayer(class APlayerController* NewPlayerController, const TSharedPtr<FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal /* = TEXT("") */);

	/* The teamnumber assigned to Players */
	int32 PlayerTeamNum;

	/* Keep reference to the night state of the previous frame */
	bool LastIsNight;

	/* The start time for the gamemode */
	int32 TimeOfDayStart;

	/* The enemy pawn class */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<class APawn> BotPawnClass;

	/* Handle for nightly bot spawning */
	FTimerHandle TimerHandle_BotSpawns;

	/* Handles bot spawning (during nighttime) */
	void SpawnBotHandler();

	/* Spawn the player next to his living coop buddy instead of a PlayerStart */
	virtual void RestartPlayer(class AController* NewPlayer) override;

	/************************************************************************/
	/* Player Spawning                                                      */
	/************************************************************************/

protected:

	/* Don't allow spectating of bots */
	virtual bool CanSpectate(APlayerController* Viewer, APlayerState* ViewTarget);

	virtual AActor* ChoosePlayerStart(AController* Player) override;

	/* Always pick a random location */
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Controller);

	virtual bool IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Controller);

	/** returns default pawn class for given controller */
	virtual UClass* GetDefaultPawnClassForController(AController* InController) override;

	/************************************************************************/
	/* Damage & Killing                                                     */
	/************************************************************************/

public: 

	virtual void Killed(AController* Killer, AController* VictimPlayer, APawn* VictimPawn, const UDamageType* DamageType);

	/* Can the player deal damage according to gamemode rules (eg. friendly-fire disabled) */
	virtual bool CanDealDamage(class ASPlayerState* DamageCauser, class ASPlayerState* DamagedPlayer) const;

	virtual float ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

	/************************************************************************/
	/* Bots                                                                 */
	/************************************************************************/

protected:

	void SpawnNewBot();

	/* Set all bots back to idle mode */
	void PassifyAllBots();

	/* Set all bots to active patroling state */
	void WakeAllBots();

public:

	/* Primary sun of the level. Assigned in Blueprint during BeginPlay (BlueprintReadWrite is required as tag instead of EditDefaultsOnly) */
	UPROPERTY(BlueprintReadWrite, Category = "DayNight")
	ADirectionalLight* PrimarySunLight;

	/************************************************************************/
	/* Scoring                                                              */
	/************************************************************************/

	/* Points awarded for surviving a night */
	UPROPERTY(EditDefaultsOnly, Category = "Scoring")
	int32 NightSurvivedScore;

};

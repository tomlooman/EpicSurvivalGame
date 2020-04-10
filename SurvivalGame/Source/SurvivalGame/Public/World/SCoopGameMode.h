// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGameMode.h"
#include "SCoopGameMode.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class SURVIVALGAME_API ASCoopGameMode : public ASGameMode
{
	GENERATED_BODY()

	ASCoopGameMode();

	/* End the match when all players are dead */
	void CheckMatchEnd();

	/* End the match, with a delay before returning to the main menu */
	void FinishMatch();

	/* Spawn the player next to his living coop buddy instead of a PlayerStart */
	virtual void RestartPlayer(class AController* NewPlayer) override;

	virtual void OnNightEnded() override;

	/* Spawn at team player if any are alive */
	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	bool bSpawnAtTeamPlayer;

	virtual void Killed(AController* Killer, AController* VictimPlayer, APawn* VictimPawn, const UDamageType* DamageType) override;

	/************************************************************************/
	/* Scoring                                                              */
	/************************************************************************/

	/* Points awarded for surviving a night */
	UPROPERTY(EditDefaultsOnly, Category = "Scoring")
	int32 ScoreNightSurvived;

};

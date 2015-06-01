// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SGameState.h"
#include "SPlayerState.h"



ASPlayerState::ASPlayerState(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	/* AI will remain in team 0, players are updated to team 1 through the GameMode::InitNewPlayer */
	TeamNumber = 0;
}


void ASPlayerState::Reset()
{
	Super::Reset();

	NumKills = 0;
	NumDeaths = 0;
	Score = 0;
}

void ASPlayerState::AddKill()
{
	NumKills++;
}

void ASPlayerState::AddDeath()
{
	NumDeaths++;
}

void ASPlayerState::ScorePoints(int32 Points)
{
	Score += Points;

	/* Add the score to the global score count */
	ASGameState* GS = Cast<ASGameState>(GetWorld()->GameState);
	if (GS)
	{
		GS->AddScore(Points);
	}
}


void ASPlayerState::SetTeamNumber(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;
}


int32 ASPlayerState::GetTeamNumber() const
{
	return TeamNumber;
}

int32 ASPlayerState::GetKills() const
{
	return NumKills;
}

int32 ASPlayerState::GetDeaths() const
{
	return NumDeaths;
}


float ASPlayerState::GetScore() const
{
	return Score;
}


void ASPlayerState::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, NumKills);
	DOREPLIFETIME(ASPlayerState, NumDeaths);
	DOREPLIFETIME(ASPlayerState, TeamNumber);
}
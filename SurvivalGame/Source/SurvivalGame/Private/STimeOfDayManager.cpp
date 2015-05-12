// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SGameState.h"
#include "STimeOfDayManager.h"


ASTimeOfDayManager::ASTimeOfDayManager()
{
	SetReplicates(true);
	bReplicateMovement = true;
}


void ASTimeOfDayManager::BeginPlay()
{
	Super::BeginPlay();
}


void ASTimeOfDayManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	/* Update the position of the sun. */
	if (PrimarySunLight)
	{
		ASGameState* MyGameState = Cast<ASGameState>(GetWorld()->GetGameState());
		if (MyGameState)
		{
			if (LastTimeOfDay == MyGameState->ElapsedGameMinutes)
			{
				TimeSinceLastIncrement += DeltaSeconds;
			}
			else
			{
				TimeSinceLastIncrement = 0;
			}

			/* Predict the movement of the sun to smooth out the rotations between replication updates of the actual time of day */
			const float PredictedIncrement = MyGameState->GetTimeOfDayIncrement() * TimeSinceLastIncrement;

			/* TimeOfDay is expressed in minutes, we need to convert this into a pitch rotation */
			const float MinutesInDay = 24 * 60;
			const float PitchOffset = 90; /* The offset to account for time of day 0 should equal midnight */
			const float PitchRotation = 360 * ((MyGameState->ElapsedGameMinutes + PredictedIncrement) / MinutesInDay);

			FRotator NewSunRotation = FRotator(PitchRotation + PitchOffset, 45.0f, 0);
			PrimarySunLight->SetActorRelativeRotation(NewSunRotation);

			LastTimeOfDay = MyGameState->ElapsedGameMinutes;
		}
	}
}


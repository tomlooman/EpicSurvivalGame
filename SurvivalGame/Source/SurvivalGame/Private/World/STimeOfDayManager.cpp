// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SGameState.h"
#include "STimeOfDayManager.h"


ASTimeOfDayManager::ASTimeOfDayManager()
{
	AmbientAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AmbientAudioComp"));
	AmbientAudioComp->bAutoActivate = false;

	/* Treshold to recapture the skylight */
	RequiredCaptureDelta = 0.01f;

	SetReplicates(true);

	/*
	
	Content/Base/BP_TimeOfDayManager extends this class and fetches all required object references from the current level.

	SkyLight, DirectionalLight (& BP_SkySphere which is controlled in Blueprint only to update sun direction in the skybox)
	
	*/
}


void ASTimeOfDayManager::BeginPlay()
{
	Super::BeginPlay();

	if (PrimarySunLight)
	{
		OriginalSunBrightness = PrimarySunLight->GetBrightness();
		TargetSunBrightness = OriginalSunBrightness;
	}

	PlayAmbientLoop();
}


void ASTimeOfDayManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ASGameState* MyGameState = Cast<ASGameState>(GetWorld()->GetGameState());
	if (MyGameState)
	{
		/* Update the position of the sun. */
		if (PrimarySunLight)
		{
			if (LastTimeOfDay == MyGameState->ElapsedGameMinutes)
			{
				TimeSinceLastIncrement += DeltaSeconds;
			}
			else
			{
				/* Reset prediction */
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

		bool CurrentNightState = MyGameState->GetIsNight();
		if (CurrentNightState != LastNightState)
		{
			if (CurrentNightState)
			{
				// Play night started cue (position is irrelevant for non spatialized & attenuated sounds)
				UGameplayStatics::PlaySoundAtLocation(this, SoundNightStarted, GetActorLocation());
				TargetSunBrightness = 0.01f;
			}
			else
			{
				// Play daytime started cue (position is irrelevant for non spatialized & attenuated sounds)
				UGameplayStatics::PlaySoundAtLocation(this, SoundNightEnded, GetActorLocation());
				TargetSunBrightness = OriginalSunBrightness;
			}

			/* Change to a new ambient loop */
			PlayAmbientLoop();
		}

		/* Update sun brightness to transition between day and night
			(Note: We cannot disable the sunlight because BP_SkySphere depends on an enabled light to update the skydome) */
		const float LerpSpeed = 0.1f * GetWorldSettings()->GetEffectiveTimeDilation();
		float CurrentSunBrightness = PrimarySunLight->GetBrightness();
		float NewSunBrightness = FMath::Lerp(CurrentSunBrightness, TargetSunBrightness, LerpSpeed);
		PrimarySunLight->SetBrightness(NewSunBrightness);

		LastNightState = CurrentNightState;
	}

	UpdateSkylight();
}


void ASTimeOfDayManager::UpdateSkylight()
{
	if (SkyLightActor)
	{
		ASGameState* MyGameState = Cast<ASGameState>(GetWorld()->GetGameState());
		if (MyGameState)
		{
			const float MinutesInDay = 24 * 60;
			/* Map the intensity from 0 - 12 - 24 hours into 0 - 1 - 0 alpha */
			const float Alpha = FMath::Sin((MyGameState->GetElapsedMinutesCurrentDay() / MinutesInDay) * 3.14);

			/* Update Intensity */
			const float NewIntensity = FMath::Lerp(0.1, 1.0, Alpha);
			SkyLightActor->GetLightComponent()->SetIntensity(NewIntensity);

			//float ColorChannel = 1.0f * NewIntensity;
			//SkyLightActor->GetLightComponent()->SetLightColor(FLinearColor(ColorChannel, ColorChannel, ColorChannel, 1.0f));

			if (RequiredCaptureDelta < FMath::Abs(NewIntensity - LastCapturedIntensity))
			{
				/* Force re-capture of the sky w/ new intensity */
				/* Hacky and costly solution to recapturing the sky, official support NYI */
				SkyLightActor->GetLightComponent()->RecaptureSky();
				LastCapturedIntensity = NewIntensity;
			}
		}
	}
}


void ASTimeOfDayManager::PlayAmbientLoop()
{
	AmbientAudioComp->Stop();

	ASGameState* MyGameState = Cast<ASGameState>(GetWorld()->GetGameState());
	if (MyGameState)
	{
		if (MyGameState->GetIsNight())
		{
			AmbientAudioComp->SetSound(AmbientNight);
		}
		else
		{
			AmbientAudioComp->SetSound(AmbientDaytime);
		}
	}

	AmbientAudioComp->Play();
}

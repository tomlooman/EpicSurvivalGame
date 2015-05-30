// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "STimeOfDayManager.generated.h"

UCLASS(ABSTRACT)
class SURVIVALGAME_API ASTimeOfDayManager : public AActor
{
	GENERATED_BODY()

	/* Cached bool of bIsNight to determine when we entered/left the night */
	bool LastNightState;

	/* Sunbrightness as specified in the level (considered as daytime sun) */
	float OriginalSunBrightness;

	/* Target brightness to lerp towards */
	float TargetSunBrightness;

protected:

	UAudioComponent* AmbientAudioComp;

public:	
	ASTimeOfDayManager();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;	

	void UpdateSkylight();

	/* Primary sun of the level. Assigned in Blueprint during BeginPlay (BlueprintReadWrite is required as tag instead of EditDefaultsOnly) */
	UPROPERTY(BlueprintReadWrite, Category = "DayNight")
	ADirectionalLight* PrimarySunLight;
	
	UPROPERTY(BlueprintReadWrite, Category = "DayNight")
	ASkyLight* SkyLightActor;

	/* Last local time of day to apply sun movement prediction */
	float LastTimeOfDay;

	/* Delta seconds since the time of day was last updated */
	float TimeSinceLastIncrement;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundNightStarted;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundNightEnded;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* AmbientDaytime;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* AmbientNight;
};

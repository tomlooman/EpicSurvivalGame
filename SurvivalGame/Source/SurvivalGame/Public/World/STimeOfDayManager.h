// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STimeOfDayManager.generated.h"

class USoundCue;

UCLASS(ABSTRACT)
class SURVIVALGAME_API ASTimeOfDayManager : public AActor
{
	GENERATED_BODY()

protected:

	ASTimeOfDayManager();

	/* Cached bool of bIsNight to determine when we entered/left the night */
	bool LastNightState;

	/* Sunbrightness as specified in the level (considered as daytime sun) */
	float OriginalSunBrightness;

	/* Target brightness to lerp towards */
	float TargetSunBrightness;

	/* Last skylight intensity that was captured */
	float LastCapturedIntensity;

	/* Maps intensity to time of day */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	class UCurveFloat* SkylightIntensityCurve;

	/* Maps skylight color to time of day */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	class UCurveVector* SkylightColorCurve;

public:	

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;	

	void UpdateSkylight();

	/* Play the ambient loop for the current time state */
	void PlayAmbientLoop();

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UAudioComponent* AmbientAudioComp;

	/* Primary sun of the level. Assigned in Blueprint during BeginPlay (BlueprintReadWrite is required as tag instead of EditDefaultsOnly) */
	UPROPERTY(BlueprintReadWrite, Category = "DayNight")
	class ADirectionalLight* PrimarySunLight;
	
	UPROPERTY(BlueprintReadWrite, Category = "DayNight")
	class ASkyLight* SkyLightActor;

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

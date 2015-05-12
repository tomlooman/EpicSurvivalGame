// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "STimeOfDayManager.generated.h"

UCLASS(ABSTRACT)
class SURVIVALGAME_API ASTimeOfDayManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ASTimeOfDayManager();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;	

	/* Primary sun of the level. Assigned in Blueprint during BeginPlay (BlueprintReadWrite is required as tag instead of EditDefaultsOnly) */
	UPROPERTY(BlueprintReadWrite, Category = "DayNight")
	ADirectionalLight* PrimarySunLight;

	/* Last local time of day to apply sun movement prediction */
	float LastTimeOfDay;

	/* Delta seconds since the time of day was last updated */
	float TimeSinceLastIncrement;
};

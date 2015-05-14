// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameState.h"
#include "SGameState.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API ASGameState : public AGameState
{
	GENERATED_BODY()

	/* Declare our own delegate to trigger and handle events for day/night event */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNightChangedDelegate, bool, bIsNight);

public:

	ASGameState(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(ReplicatedUsing=OnRep_NightChanged)
	bool bIsNight;

	UFUNCTION()
	void OnRep_NightChanged();

	/* Current time of day in the gamemode represented in full minutes */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "TimeOfDay")
	int32 ElapsedGameMinutes;

	/* Conversion of 1 second real time to X seconds gametime of the day/night cycle */
	UPROPERTY(EditDefaultsOnly, Category = "TimeOfDay")
	float TimeScale;

	/* Returns the time of day increment every real second (converted to accelerated game time, eg. 1 real second is 1 minute in time of day increment) */
	float GetTimeOfDayIncrement();

	/* By passing in "exec" we expose it as a command line (press ~ to open) */
	UFUNCTION(exec)
	void SetTimeOfDay(float NewTimeOfDay);

	int32 GetElapsedDays();

	/* Returns whole days elapsed, represented in minutes */
	int32 GetElapsedDaysInMinutes();

	/* Classes can hook functions to this delegate to they may respond with their own logic, eg. to start spawning enemies during the night */
	FNightChangedDelegate OnNightChanged;

	/* NetMulticast will send this event to all clients that know about this object, in the case of GameState that means every client. */
	UFUNCTION(Reliable, NetMulticast)
	void BroadcastGameMessage(const FString& NewMessage);
};

// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SPlayerController.h"
#include "SHUD.h"
#include "SGameState.h"


ASGameState::ASGameState(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/* 1 minute real time is 15 minutes game time */
	TimeScale = 15.0f;
	bIsNight = false;
}


void ASGameState::SetTimeOfDay(float NewTimeOfDay)
{
	ElapsedGameMinutes = NewTimeOfDay;
}


float ASGameState::GetTimeOfDayIncrement()
{
	return (GetWorldSettings()->GetEffectiveTimeDilation() * TimeScale);
}


void ASGameState::OnRep_NightChanged()
{
	// TODO: Repurpose or remove.
}


int32 ASGameState::GetElapsedDays()
{
	const float MinutesInDay = 24 * 60;
	const float ElapsedDays = ElapsedGameMinutes / MinutesInDay;
	return FMath::FloorToInt(ElapsedDays);
}


int32 ASGameState::GetElapsedDaysInMinutes()
{
	const int32 MinutesInDay = 24 * 60;
	return GetElapsedDays() * MinutesInDay;
}


/* As with Server side functions, NetMulticast functions have a _Implementation body */
void ASGameState::BroadcastGameMessage_Implementation(const FString& NewMessage)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		ASPlayerController* MyController = Cast<ASPlayerController>(*It);
		if (MyController && MyController->IsLocalController())
		{
			ASHUD* MyHUD = Cast<ASHUD>(MyController->GetHUD());
			if (MyHUD)
			{
				MyHUD->MessageReceived(NewMessage);
			}
		}
	}
}


void ASGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameState, ElapsedGameMinutes);
	DOREPLIFETIME(ASGameState, bIsNight);
}

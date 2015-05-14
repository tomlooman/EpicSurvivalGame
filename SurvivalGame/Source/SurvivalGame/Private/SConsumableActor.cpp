// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SConsumableActor.h"
#include "SCharacter.h"


ASConsumableActor::ASConsumableActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/* A default to tweak per food variation in Blueprint */
	Nutrition = 20;

	bAllowRespawn = true;
	RespawnDelay = 120.0f;
	RespawnDelayRange = 120.0f;
}


void ASConsumableActor::OnUsed(APawn* InstigatorPawn)
{
	ASCharacter* Pawn = Cast<ASCharacter>(InstigatorPawn);
	if (Pawn)
	{
		Pawn->ConsumeFood(Nutrition);
	}

	Super::OnUsed(InstigatorPawn);
}

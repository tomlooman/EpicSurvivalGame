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
	RespawnDelay = 60.0f;
	RespawnDelayRange = 20.0f;
}


void ASConsumableActor::OnUsed(APawn* InstigatorPawn)
{
	ASCharacter* Pawn = Cast<ASCharacter>(InstigatorPawn);
	if (Pawn)
	{
		/* Restore some hitpoints and energy (hunger) */
		Pawn->RestoreCondition(Nutrition * 0.5f, Nutrition);
	}

	Super::OnUsed(InstigatorPawn);
}

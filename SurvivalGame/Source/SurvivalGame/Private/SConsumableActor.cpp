// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SConsumableActor.h"
#include "SCharacter.h"


ASConsumableActor::ASConsumableActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// A default to tweak per food variation in Blueprint
	Nutrition = 25;
}


void ASConsumableActor::OnUsed(APawn* InstigatorPawn)
{
	// Plays pickup sound from base class
	Super::OnUsed(InstigatorPawn);

	ASCharacter* Pawn = Cast<ASCharacter>(InstigatorPawn);
	if (Pawn)
	{
		Pawn->ConsumeFood(Nutrition);
	}

	// Remove from level
	Destroy();
}

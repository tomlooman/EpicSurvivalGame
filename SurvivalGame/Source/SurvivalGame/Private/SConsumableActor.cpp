// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SConsumableActor.h"
#include "SCharacter.h"


ASConsumableActor::ASConsumableActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// A default to tweak per item variation in Blueprint
	EnergyToRestore = 25;
}


void ASConsumableActor::OnUsed(APawn* InstigatorPawn)
{
	// Plays pickup sound from base class
	Super::OnUsed(InstigatorPawn);

	ASCharacter* Pawn = Cast<ASCharacter>(InstigatorPawn);
	if (Pawn)
	{
		Pawn->RestoreEnergy(EnergyToRestore);

		// TODO: replace with HUD event message. Pawn>PC>GetHUD()>..
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Item consumed!");
	}

	// Remove from level
	Destroy();
}

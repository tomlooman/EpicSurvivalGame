// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SCharacter.h"
#include "SWeapon.h"
#include "SWeaponPickup.h"
#include "SPlayerController.h"
#include "SHUD.h"


ASWeaponPickup::ASWeaponPickup(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bAllowRespawn = false;

	/* Enabled to support simulated physics movement when weapons are dropped by a player */
	bReplicateMovement = true;
}


void ASWeaponPickup::OnUsed(APawn* InstigatorPawn)
{
	ASCharacter* MyPawn = Cast<ASCharacter>(InstigatorPawn);
	if (MyPawn)
	{
		/* Fetch the default variables of the class we are about to pick up and check if the storage slot is available on the pawn. */
		if (MyPawn->WeaponSlotAvailable(WeaponClass->GetDefaultObject<ASWeapon>()->GetStorageSlot()))
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.bNoCollisionFail = true;
			ASWeapon* NewWeapon = GetWorld()->SpawnActor<ASWeapon>(WeaponClass, SpawnInfo);

			MyPawn->AddWeapon(NewWeapon);

			Super::OnUsed(InstigatorPawn);
		}
		else
		{
			ASPlayerController* PC = Cast<ASPlayerController>(MyPawn->GetController());
			if (PC)
			{
				PC->ClientMessageReceived("Weapon slot already taken.");
			}
		}
	}
}



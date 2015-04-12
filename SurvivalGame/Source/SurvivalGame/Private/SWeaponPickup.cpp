// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SCharacter.h"
#include "SWeapon.h"
#include "SWeaponPickup.h"



ASWeaponPickup::ASWeaponPickup(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/* ObjectInitializer from constructor is no longer required to create components. */
// 	SkelMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkelMesh"));
// 	SkelMeshComp->SetSimulatePhysics(true);
// 	SkelMeshComp->AttachParent = GetRootComponent();
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

			Destroy();
		}
		{
			// TODO: Add warning to user HUD.
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Weapon slot taken!");
		}
	}
}



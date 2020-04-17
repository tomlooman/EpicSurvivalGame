// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Mutators/SMutator_WeaponReplacement.h"
#include "World/SGameMode.h"
#include "Items/SWeaponPickup.h"



bool ASMutator_WeaponReplacement::CheckRelevance_Implementation(AActor* Other)
{
	ASWeaponPickup* WeaponPickup = Cast<ASWeaponPickup>(Other);
	if (WeaponPickup)
	{
		for (int32 i = 0; i < WeaponsToReplace.Num(); i++)
		{
			const FReplacementInfo& Info = WeaponsToReplace[i];

			if (Info.FromWeapon == WeaponPickup->WeaponClass)
			{
				WeaponPickup->WeaponClass = Info.ToWeapon;
			}
		}
	}

	/* Always call Super so we can run the entire chain of linked Mutators. */
	return Super::CheckRelevance_Implementation(Other);
}


void ASMutator_WeaponReplacement::InitGame_Implementation(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	/* Update default inventory weapons for current gamemode. */
	ASGameMode* GameMode = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode != nullptr)
	{
		for (int32 i = 0; i < GameMode->DefaultInventoryClasses.Num(); i++)
		{
			for (int32 j = 0; j < WeaponsToReplace.Num(); j++)
			{
				FReplacementInfo Info = WeaponsToReplace[j];
				if (GameMode->DefaultInventoryClasses[i] == Info.FromWeapon)
				{
					GameMode->DefaultInventoryClasses[i] = Info.ToWeapon;
				}
			}
		}
	}

	Super::InitGame_Implementation(MapName, Options, ErrorMessage);
}

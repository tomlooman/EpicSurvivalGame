// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SPickupActor.h"
#include "SConsumableActor.generated.h"

/**
 * Base class for consumable (food) items. Consumable by players to restore energy
 */
UCLASS(ABSTRACT)
class SURVIVALGAME_API ASConsumableActor : public ASPickupActor
{
	GENERATED_BODY()

	ASConsumableActor(const FObjectInitializer& ObjectInitializer);

protected:

	/* Consume item, restoring energy to player */
	virtual void OnUsed(APawn* InstigatorPawn) override;

	/* Amount of hitpoints restored and hunger reduced when consumed. */
	UPROPERTY(EditDefaultsOnly, Category = "Consumable")
	float Nutrition;
};

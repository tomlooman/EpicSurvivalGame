// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SUsableActor.h"
#include "SPickupActor.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class SURVIVALGAME_API ASPickupActor : public ASUsableActor
{
	GENERATED_UCLASS_BODY()

	void BeginPlay() override;

	void OnUsed(APawn* InstigatorPawn) override;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* PickupSound;
};

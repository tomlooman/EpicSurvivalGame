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

	virtual void OnUsed(APawn* InstigatorPawn) override;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* PickupSound;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsActive)
	bool bIsActive;

	UFUNCTION()
	void OnRep_IsActive();

protected:

	virtual void RespawnPickup();

	virtual void OnPickedUp();

	virtual void OnRespawned();

public:

	/* Immediately spawn on begin play */
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	bool bStartActive;

	/* Will this item ever respawn */
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	bool bAllowRespawn;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	float RespawnDelay;

	/* Extra delay randomly applied on the respawn timer */
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	float RespawnDelayRange;
};

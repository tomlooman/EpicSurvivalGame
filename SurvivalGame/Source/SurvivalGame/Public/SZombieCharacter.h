// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SBaseCharacter.h"
#include "SZombieCharacter.generated.h"

UCLASS(ABSTRACT)
class SURVIVALGAME_API ASZombieCharacter : public ASBaseCharacter
{
	GENERATED_BODY()

	/* Last time the player was spotted - to clear target after a few sections */
	float LastSeenTime;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	class UPawnSensingComponent* PawnSensingComp;

	virtual void BeginPlay() override;

protected:

	/* Triggered by pawn sensing component when a pawn is spotted */
	/* When using functions as delegates they need to be marked with UFUNCTION(). We assign this function to FSeePawnDelegate */
	UFUNCTION()
	void OnSeePlayer(APawn* Pawn);
	
	UFUNCTION()
	void OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume);

public:

	ASZombieCharacter(const class FObjectInitializer& ObjectInitializer);
	
	/* The thinking part of the brain, steers our zombie and makes decisions based on the data we feed it from the Blackboard */
	/* Assigned at the Character level (instead of Controller) so we may use different zombie behaviors while re-using one controller. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;

};

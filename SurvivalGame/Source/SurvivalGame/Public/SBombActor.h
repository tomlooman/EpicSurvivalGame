// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SUsableActor.h"
#include "SBombActor.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API ASBombActor : public ASUsableActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(VisibleDefaultsOnly)
	UParticleSystemComponent* ExplosionPCS;

	UPROPERTY(VisibleDefaultsOnly)
	UParticleSystemComponent* FuzePCS;

	UPROPERTY(VisibleDefaultsOnly)
	UAudioComponent* AudioComp;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Effects")
	UParticleSystem* ExplosionFX;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Effects")
	UParticleSystem* FuzeFX;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Effects")
	USoundCue* ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Effects")
	USoundCue* FuzeSound;

	/* Is fuze lit and counting down */
	bool bIsFuzeActive;

	/* Initial time on the fuze */
	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Settings")
	float MaxFuzeTime;
	
	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Settings")
	float ExplosionDamage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Settings")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Settings")
	TSubclassOf<UDamageType> DamageType;

	virtual void BeginPlay() override;

	/* Activates the bomb fuze */
	virtual void OnUsed(APawn* InstigatorPawn) override;
	
	/* Explode the bomb */
	void OnExplode();
};

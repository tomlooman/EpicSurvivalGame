// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "SImpactEffect.generated.h"

UCLASS(ABSTRACT, Blueprintable)
class SURVIVALGAME_API ASImpactEffect : public AActor
{
	GENERATED_BODY()

protected:

	UParticleSystem* GetImpactFX(EPhysicalSurface SurfaceType) const;

	USoundCue* GetImpactSound(EPhysicalSurface SurfaceType) const;
	
public:	

	ASImpactEffect();

	virtual void PostInitializeComponents() override;

	/* FX spawned on standard materials */
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* DefaultFX;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* FleshFX;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* DefaultSound;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* FleshSound;

	FHitResult SurfaceHit;
};

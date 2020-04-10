// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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
	UParticleSystem* PlayerFleshFX;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* ZombieFleshFX;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* DefaultSound;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* PlayerFleshSound;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* ZombieFleshSound;

	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	UMaterial* DecalMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	float DecalSize;

	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	float DecalLifeSpan;

	FHitResult SurfaceHit;
};

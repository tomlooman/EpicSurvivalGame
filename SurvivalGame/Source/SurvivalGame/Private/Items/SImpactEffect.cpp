// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#include "Items/SImpactEffect.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/DecalComponent.h"
#include "SurvivalGame/SurvivalGame.h"



ASImpactEffect::ASImpactEffect()
{
	SetAutoDestroyWhenFinished(true);
	/* Can ever tick is required to trigger bAutoDestroyWhenFinished, which is checked in AActor::Tick */
	PrimaryActorTick.bCanEverTick = true;

	DecalLifeSpan = 10.0f;
	DecalSize = 16.0f;
}


void ASImpactEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/* Figure out what we hit (SurfaceHit is setting during actor instantiation in weapon class) */
	UPhysicalMaterial* HitPhysMat = SurfaceHit.PhysMaterial.Get();
	EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

	UParticleSystem* ImpactFX = GetImpactFX(HitSurfaceType);
	if (ImpactFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, GetActorLocation(), GetActorRotation());
	}

	USoundCue* ImpactSound = GetImpactSound(HitSurfaceType);
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	if (DecalMaterial)
	{
		FVector ImpactNormal = SurfaceHit.ImpactNormal;
		ImpactNormal.Normalize();
		/* Inverse to point towards the wall. Invert to get the correct orientation of the decal (pointing into the surface instead of away, messing with the normals, and lighting) */
		ImpactNormal = -ImpactNormal;

		FRotator RandomDecalRotation = ImpactNormal.ToOrientationRotator();
		RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);

		UDecalComponent* DecalComp = UGameplayStatics::SpawnDecalAttached(DecalMaterial, FVector(DecalSize, DecalSize, DecalSize),
			SurfaceHit.Component.Get(), SurfaceHit.BoneName,
			SurfaceHit.ImpactPoint, RandomDecalRotation, EAttachLocation::KeepWorldPosition,
			DecalLifeSpan);

		if (DecalComp)
		{
			DecalComp->SetFadeOut(DecalLifeSpan, 0.5f, false);
		}
	}
}


UParticleSystem* ASImpactEffect::GetImpactFX(EPhysicalSurface SurfaceType) const
{
	switch (SurfaceType)
	{
	case SURFACE_DEFAULT:
		return DefaultFX;
	case SURFACE_FLESH:
		return PlayerFleshFX;
	case SURFACE_ZOMBIEBODY:
	case SURFACE_ZOMBIEHEAD:
	case SURFACE_ZOMBIELIMB:
		return ZombieFleshFX;
	default:
		return nullptr;
	}
}


USoundCue* ASImpactEffect::GetImpactSound(EPhysicalSurface SurfaceType) const
{
	switch (SurfaceType)
	{
	case SURFACE_DEFAULT:
		return DefaultSound;
	case SURFACE_FLESH:
		return PlayerFleshSound;
	case SURFACE_ZOMBIEBODY:
	case SURFACE_ZOMBIEHEAD:
	case SURFACE_ZOMBIELIMB:
		return ZombieFleshSound;
	default:
		return nullptr;
	}
}

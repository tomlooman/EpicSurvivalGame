// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SBombActor.h"


ASBombActor::ASBombActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FuzePCS = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("Fuze"));
	FuzePCS->bAutoActivate = false;
	FuzePCS->bAutoDestroy = false;
	FuzePCS->AttachParent = RootComponent;

	ExplosionPCS = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("Explosion"));
	ExplosionPCS->bAutoActivate = false;
	ExplosionPCS->bAutoDestroy = false;
	ExplosionPCS->AttachParent = RootComponent;

	AudioComp = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("AudioComp"));
	AudioComp->bAutoActivate = false;
	AudioComp->bAutoDestroy = false;
	AudioComp->AttachParent = RootComponent;

	// Let the bomb be thrown and roll around
	MeshComp->SetSimulatePhysics(true);

	MaxFuzeTime = 5.0f;
	ExplosionDamage = 100;
	ExplosionRadius = 512;
}


void ASBombActor::BeginPlay()
{
	Super::BeginPlay();
}


void ASBombActor::OnUsed(APawn* InstigatorPawn)
{
	Super::OnUsed(InstigatorPawn);
	// TODO: Setup instigator controller (we might require this for ApplyRadialDamage)

	if (!bIsFuzeActive)
	{
		bIsFuzeActive = true;

		if (FuzeSound)
		{
			AudioComp->SetSound(FuzeSound);
			AudioComp->FadeIn(0.25f);
		}
		if (FuzeFX)
		{
			FuzePCS->SetTemplate(FuzeFX);
			FuzePCS->ActivateSystem();
		}

		// Active the fuze to explode the bomb after several seconds
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASBombActor::OnExplode, MaxFuzeTime, false);
	}
}


void ASBombActor::OnExplode()
{
	// First deactivate all running fuze effects
	FuzePCS->DeactivateSystem();
	AudioComp->Stop();

	MeshComp->SetVisibility(false, false);

	// Active all explosion effects
	if (ExplosionSound)
	{
		AudioComp->SetSound(ExplosionSound);
		AudioComp->Play();
	}
	if (ExplosionFX)
	{
		ExplosionPCS->SetTemplate(ExplosionFX);
		ExplosionPCS->ActivateSystem();
	}

	// Apply damage to player, enemies and environmental objects
	TArray<AActor*> IgnoreActors;
	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, DamageType, IgnoreActors, this, NULL);

	// TODO: Deal Damage to objects that support it

	// TODO: Apply radial impulse to supporting objects

	// TODO: Prepare to destroy self
}

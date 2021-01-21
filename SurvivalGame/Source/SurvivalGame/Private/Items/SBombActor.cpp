// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#include "Items/SBombActor.h"
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


ASBombActor::ASBombActor()
{
	FuzePCS = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Fuze"));
	FuzePCS->bAutoActivate = false;
	FuzePCS->bAutoDestroy = false;
	FuzePCS->SetupAttachment(RootComponent);

	ExplosionPCS = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Explosion"));
	ExplosionPCS->bAutoActivate = false;
	ExplosionPCS->bAutoDestroy = false;
	ExplosionPCS->SetupAttachment(RootComponent);

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComp->bAutoActivate = false;
	AudioComp->bAutoDestroy = false;
	AudioComp->SetupAttachment(RootComponent);

	// Let the bomb be thrown and roll around
	MeshComp->SetSimulatePhysics(true);

	MaxFuzeTime = 3.0f;
	ExplosionDamage = 200;
	ExplosionRadius = 600;

	SetReplicates(true);
	SetReplicatingMovement(true);
}


void ASBombActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Ensure the fuze timer is cleared
	GetWorld()->GetTimerManager().ClearTimer(FuzeTimerHandle);

	// Alternatively you can clear ALL timers.
	/*GetWorld()->GetTimerManager().ClearAllTimersForObject(this);*/
}


void ASBombActor::OnUsed(APawn* InstigatorPawn)
{
	if (bIsFuzeActive)
	{
		return;
	}

	Super::OnUsed(InstigatorPawn);

	bIsFuzeActive = true;
	// Runs on all clients (NetMulticast)
	SimulateFuzeFX();

	// Activate the fuze to explode the bomb after several seconds
	GetWorldTimerManager().SetTimer(FuzeTimerHandle, this, &ASBombActor::Explode, MaxFuzeTime, false);

}


void ASBombActor::Explode()
{
	if (bExploded)
	{
		return;
	}

	bExploded = true;
	// Runs on all clients (NetMulticast)
	SimulateExplosion();

	// Apply damage to player, enemies and environmental objects
	TArray<AActor*> IgnoreActors;
	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, DamageType, IgnoreActors, this, nullptr);

	//DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 32, FColor::Red, false, 1.5f);

	/* Allow clients to show FX before deleting */
	SetLifeSpan(2.0f);
}


void ASBombActor::SimulateFuzeFX_Implementation()
{
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
}


void ASBombActor::SimulateExplosion_Implementation()
{
	// First deactivate all running fuze effects
	FuzePCS->DeactivateSystem();
	AudioComp->Stop();

	MeshComp->SetVisibility(false, false);

	// Activate all explosion effects
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

	// For Blueprint
	K2_SimulateExplosion();
}

float ASBombActor::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	Explode();

	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

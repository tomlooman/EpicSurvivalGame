// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SUsableActor.h"
#include "SBombActor.generated.h"

class USoundCue;
class UParticleSystem;
class UAudioComponent;
class UParticleSystemComponent;

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API ASBombActor : public ASUsableActor
{
	GENERATED_BODY()

public:

	ASBombActor();

protected:

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/* Explode when taking damage*/
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

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

	bool bExploded;

	/* Initial time on the fuze */
	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Settings")
	float MaxFuzeTime;
	
	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Settings")
	float ExplosionDamage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Settings")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb|Settings")
	TSubclassOf<UDamageType> DamageType;

	/* Handle to manage the timer */
	FTimerHandle FuzeTimerHandle;

	/* Activates the bomb fuze */
	virtual void OnUsed(APawn* InstigatorPawn) override;
	
	/* Explode the bomb */
	void Explode();

	/* Blueprint function */
	UFUNCTION(BlueprintImplementableEvent)
	void K2_SimulateExplosion();

	/* Note: Marked unreliable as its non-gameplay VFX only */
	UFUNCTION(Unreliable, NetMulticast)
	void SimulateFuzeFX();

	/* Note: Marked unreliable as its non-gameplay VFX only */
	UFUNCTION(Unreliable, NetMulticast)
	void SimulateExplosion();
};

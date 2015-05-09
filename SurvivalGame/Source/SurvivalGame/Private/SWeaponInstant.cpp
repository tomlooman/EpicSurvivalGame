// Fill out your copyright notice in the Description page of Project Settings.

#include "SurvivalGame.h"
#include "SWeaponInstant.h"
#include "SImpactEffect.h"
#include "SPlayerController.h"


ASWeaponInstant::ASWeaponInstant(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	HitDamage = 26;
	WeaponRange = 10000;

	AllowedViewDotHitDir = 0.8f;
	ClientSideHitLeeway = 200.0f;
	MinimumProjectileSpawnDistance = 800;
	TracerRoundInterval = 3;
	TimeBetweenShots = 0.1f;
}


void ASWeaponInstant::FireWeapon()
{	
	const FVector AimDir = GetAdjustedAim();
	const FVector StartPos = GetCameraDamageStartLocation(AimDir);
	const FVector EndPos = StartPos + (AimDir * WeaponRange);

	const FHitResult Impact = WeaponTrace(StartPos, EndPos);
	ProcessInstantHit(Impact, StartPos, AimDir);
}


bool ASWeaponInstant::ShouldDealDamage(AActor* TestActor) const
{
	// If we are an actor on the server, or the local client has authoritative control over actor, we should register damage.
	if (TestActor)
	{
		if (GetNetMode() != NM_Client ||
			TestActor->Role == ROLE_Authority ||
			TestActor->bTearOff)
		{
			return true;
		}
	}

	return false;
}


void ASWeaponInstant::DealDamage(const FHitResult& Impact, const FVector& ShootDir)
{
	FPointDamageEvent PointDmg;
	PointDmg.DamageTypeClass = DamageType;
	PointDmg.HitInfo = Impact;
	PointDmg.ShotDirection = ShootDir;
	PointDmg.Damage = HitDamage;

	Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, MyPawn->Controller, this);
}


void ASWeaponInstant::ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir)
{
	if (MyPawn && MyPawn->IsLocallyControlled() && GetNetMode() == NM_Client)
	{
		// If we are a client and hit something that is controlled by server
		if (Impact.GetActor() && Impact.GetActor()->GetRemoteRole() == ROLE_Authority)
		{
			// Notify the server of our local hit to validate and apply actual hit damage.
			ServerNotifyHit(Impact, ShootDir);
		}
		else if (Impact.GetActor() == nullptr)
		{
			if (Impact.bBlockingHit)
			{
				ServerNotifyHit(Impact, ShootDir);
			}
			else
			{
				ServerNotifyMiss(ShootDir);
			}
		}
	}

	// Process a confirmed hit.
	ProcessInstantHitConfirmed(Impact, Origin, ShootDir);
}


void ASWeaponInstant::ProcessInstantHitConfirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir)
{
	// Handle damage
	if (ShouldDealDamage(Impact.GetActor()))
	{
		DealDamage(Impact, ShootDir);
	}

	// Play FX on remote clients
	if (Role == ROLE_Authority)
	{
		HitOriginNotify = Origin;
	}

	// Play FX locally
	if (GetNetMode() != NM_DedicatedServer)
	{
		SimulateInstantHit(Origin);
	}
}


void ASWeaponInstant::SimulateInstantHit(const FVector& Origin)
{
	const FVector StartTrace = Origin;
	const FVector AimDir = GetAdjustedAim();
	const FVector EndTrace = StartTrace + (AimDir * WeaponRange);

 	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	if (Impact.bBlockingHit)
	{
		SpawnImpactEffects(Impact);
		SpawnTrailEffects(Impact.ImpactPoint);
	}
	else
	{
		SpawnTrailEffects(EndTrace);
	}
}


bool ASWeaponInstant::ServerNotifyHit_Validate(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir)
{
	return true;
}


void ASWeaponInstant::ServerNotifyHit_Implementation(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir)
{
	// If we have an instigator, calculate the dot between the view and the shot
	if (Instigator && (Impact.GetActor() || Impact.bBlockingHit))
	{
		const FVector Origin = GetMuzzleLocation();
		const FVector ViewDir = (Impact.Location - Origin).GetSafeNormal();

		const float ViewDotHitDir = FVector::DotProduct(Instigator->GetViewRotation().Vector(), ViewDir);
		if (ViewDotHitDir > AllowedViewDotHitDir)
		{
			// TODO: Check for weapon state

			if (Impact.GetActor() == nullptr)
			{
				if (Impact.bBlockingHit)
				{
					ProcessInstantHitConfirmed(Impact, Origin, ShootDir);
				}
			}
			// Assume it told the truth about static things because we don't move and the hit
			// usually doesn't have significant gameplay implications
			else if (Impact.GetActor()->IsRootComponentStatic() || Impact.GetActor()->IsRootComponentStationary())
			{
				ProcessInstantHitConfirmed(Impact, Origin, ShootDir);
			}
			else
			{
				const FBox HitBox = Impact.GetActor()->GetComponentsBoundingBox();

				FVector BoxExtent = 0.5 * (HitBox.Max - HitBox.Min);
				BoxExtent *= ClientSideHitLeeway;

				BoxExtent.X = FMath::Max(20.0f, BoxExtent.X);
				BoxExtent.Y = FMath::Max(20.0f, BoxExtent.Y);
				BoxExtent.Z = FMath::Max(20.0f, BoxExtent.Z);

				const FVector BoxCenter = (HitBox.Min + HitBox.Max) * 0.5;

				// If we are within client tolerance
				if (FMath::Abs(Impact.Location.Z - BoxCenter.Z) < BoxExtent.Z &&
					FMath::Abs(Impact.Location.X - BoxCenter.X) < BoxExtent.X &&
					FMath::Abs(Impact.Location.Y - BoxCenter.Y) < BoxExtent.Y)
				{
					ProcessInstantHitConfirmed(Impact, Origin, ShootDir);
				}
			}
		}
	}

	// TODO: UE_LOG on failures & rejection
}


bool ASWeaponInstant::ServerNotifyMiss_Validate(FVector_NetQuantizeNormal ShootDir)
{
	return true;
}


void ASWeaponInstant::ServerNotifyMiss_Implementation(FVector_NetQuantizeNormal ShootDir)
{
	const FVector Origin = GetMuzzleLocation();

	// Play on remote clients
	HitOriginNotify = Origin;

	const FVector EndTrace = Origin + (ShootDir * WeaponRange);
	if (GetNetMode() != NM_DedicatedServer)
	{
		SpawnTrailEffects(EndTrace);
	}
}


void ASWeaponInstant::SpawnImpactEffects(const FHitResult& Impact)
{
	if (ImpactTemplate && Impact.bBlockingHit)
	{
		// TODO: Possible re-trace to get hit component that is lost during replication.

		/* This function prepares an actor to spawn, but requires another call to finish the actual spawn progress. This allows manipulation of properties before entering into the level */
		ASImpactEffect* EffectActor = GetWorld()->SpawnActorDeferred<ASImpactEffect>(ImpactTemplate, Impact.ImpactPoint, Impact.ImpactPoint.Rotation());
		if (EffectActor)
		{
			EffectActor->SurfaceHit = Impact;
			UGameplayStatics::FinishSpawningActor(EffectActor, FTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint));
		}
	}
}


void ASWeaponInstant::SpawnTrailEffects(const FVector& EndPoint)
{
	// Keep local count for effects
	BulletsShotCount++;

	const FVector Origin = GetMuzzleLocation();
	FVector ShootDir = EndPoint - Origin;

	// Only spawn if a minimum distance is satisfied.
	if (ShootDir.Size() < MinimumProjectileSpawnDistance)
		return;

	if (BulletsShotCount % TracerRoundInterval == 0)
	{
		if (TracerFX)
		{
			ShootDir.Normalize();
			UGameplayStatics::SpawnEmitterAtLocation(this, TracerFX, Origin, ShootDir.Rotation());
		}
	}
	else 
	{
		// Ignore trails for self created trails.
		ASCharacter* OwningPawn = GetPawnOwner();
		if (OwningPawn && OwningPawn->IsLocallyControlled())
			return;

		if (TrailFX)
		{
			UParticleSystemComponent* TrailPSC = UGameplayStatics::SpawnEmitterAtLocation(this, TrailFX, Origin);
			if (TrailPSC)
			{
				TrailPSC->SetVectorParameter(TrailTargetParam, EndPoint);
			}
		}
	}
}


void ASWeaponInstant::OnRep_HitLocation()
{
	// Played on all remote clients
	SimulateInstantHit(HitOriginNotify);
}


void ASWeaponInstant::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASWeaponInstant, HitOriginNotify, COND_SkipOwner);
}
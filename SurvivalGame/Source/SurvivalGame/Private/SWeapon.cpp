// Fill out your copyright notice in the Description page of Project Settings.

#include "SurvivalGame.h"
#include "SWeapon.h"
#include "SCharacter.h"
#include "STypes.h"
#include "SWeaponPickup.h"
#include "SPlayerController.h"


ASWeapon::ASWeapon(const class FObjectInitializer& PCIP)
: Super(PCIP)
{
	Mesh = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh3P"));
	Mesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh->bChartDistanceFactor = true;
	Mesh->bReceivesDecals = true;
	Mesh->CastShadow = true;
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	RootComponent = Mesh;

	bIsEquipped = false;
	CurrentState = EWeaponState::Idle;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	//bReplicateInstigator = true;
	bNetUseOwnerRelevancy = true;

	MuzzleAttachPoint = TEXT("MuzzleFlashSocket");
	StorageSlot = EInventorySlot::Primary;
}


void ASWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	DetachMeshFromPawn();
	StopSimulatingWeaponFire();
}


/*
	Return Mesh of Weapon
*/
USkeletalMeshComponent* ASWeapon::GetWeaponMesh() const
{
	return Mesh;
}


class ASCharacter* ASWeapon::GetPawnOwner() const
{
	return MyPawn;
}


void ASWeapon::SetOwningPawn(ASCharacter* NewOwner)
{
	if (MyPawn != NewOwner)
	{
		Instigator = NewOwner;
		MyPawn = NewOwner;
		// Net owner for RPC calls.
		SetOwner(NewOwner);
	}
}


void ASWeapon::OnRep_MyPawn()
{
	if (MyPawn)
	{
		OnEnterInventory(MyPawn);
	}
	else
	{
		OnLeaveInventory();

	}
}


void ASWeapon::AttachMeshToPawn(EInventorySlot Slot)
{
	if (MyPawn)
	{
		// Remove and hide
		DetachMeshFromPawn();

		USkeletalMeshComponent* PawnMesh = MyPawn->GetMesh();
		FName AttachPoint = MyPawn->GetInventoryAttachPoint(Slot);
		Mesh->SetHiddenInGame(false);
		Mesh->AttachTo(PawnMesh, AttachPoint, EAttachLocation::SnapToTarget);
	}
}


void ASWeapon::DetachMeshFromPawn()
{
	Mesh->DetachFromParent();
	Mesh->SetHiddenInGame(true);	
}


void ASWeapon::OnEquip()
{
	AttachMeshToPawn();

	bPendingEquip = true;
	DetermineWeaponState();

	float Duration = PlayWeaponAnimation(EquipAnim);
	if (Duration <= 0.0f)
	{
		// Failsafe
		Duration = 0.5f;
	}
	EquipStartedTime = GetWorld()->TimeSeconds;
	EquipDuration = Duration;

	GetWorldTimerManager().SetTimer(EquipFinishedTimerHandle, this, &ASWeapon::OnEquipFinished, Duration, false);

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		PlayWeaponSound(EquipSound);
	}
}


void ASWeapon::OnUnEquip()
{
	AttachMeshToPawn(StorageSlot);
	bIsEquipped = false;
	StopFire();

	if (bPendingEquip)
	{
		StopWeaponAnimation(EquipAnim);
		bPendingEquip = false;

		GetWorldTimerManager().ClearTimer(EquipFinishedTimerHandle);
	}

	DetermineWeaponState();
}


void ASWeapon::OnEnterInventory(ASCharacter* NewOwner)
{
	SetOwningPawn(NewOwner);
	AttachMeshToPawn(StorageSlot);
}


void ASWeapon::OnLeaveInventory()
{
	if (Role == ROLE_Authority)
	{
		SetOwningPawn(nullptr);
	}

	if (IsAttachedToPawn())
	{
		OnUnEquip();
	}

	DetachMeshFromPawn();
}


bool ASWeapon::IsEquipped() const
{
	return bIsEquipped;
}


bool ASWeapon::IsAttachedToPawn() const // TODO: Review name to more accurately specify meaning.
{
	return bIsEquipped || bPendingEquip;
}


void ASWeapon::StartFire()
{
	if (Role < ROLE_Authority)
	{
		ServerStartFire();
	}

	if (!bWantsToFire)
	{
		bWantsToFire = true;
		DetermineWeaponState();
	}
}


void ASWeapon::StopFire()
{
	if (Role < ROLE_Authority)
	{
		ServerStopFire();
	}

	if (bWantsToFire)
	{
		bWantsToFire = false;
		DetermineWeaponState();
	}
}


bool ASWeapon::ServerStartFire_Validate()
{
	return true;
}


void ASWeapon::ServerStartFire_Implementation()
{
	StartFire();
}


bool ASWeapon::ServerStopFire_Validate()
{
	return true;
}


void ASWeapon::ServerStopFire_Implementation()
{
	StopFire();
}


bool ASWeapon::CanFire() const
{
	bool bPawnCanFire = MyPawn && MyPawn->CanFire();
	bool bStateOK = CurrentState == EWeaponState::Idle || CurrentState == EWeaponState::Firing;
	return bPawnCanFire && bStateOK;
}


FVector ASWeapon::GetAdjustedAim() const
{
	ASPlayerController* const PC = Instigator ? Cast<ASPlayerController>(Instigator->Controller) : nullptr;
	FVector FinalAim = FVector::ZeroVector;

	if (PC)
	{
		FVector CamLoc;
		FRotator CamRot;
		PC->GetPlayerViewPoint(CamLoc, CamRot);

		FinalAim = CamRot.Vector();
	}
	else if (Instigator)
	{
		FinalAim = Instigator->GetBaseAimRotation().Vector();
	}

	return FinalAim;
}


FVector ASWeapon::GetCameraDamageStartLocation(const FVector& AimDir) const
{
	ASPlayerController* PC = MyPawn ? Cast<ASPlayerController>(MyPawn->Controller) : nullptr;
	FVector OutStartTrace = FVector::ZeroVector;

	if (PC)
	{
		FRotator dummyRot;
		PC->GetPlayerViewPoint(OutStartTrace, dummyRot);

		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		// TODO: Break down into easy to understand code (copied from ShooterGame)
		OutStartTrace = OutStartTrace + AimDir * ((Instigator->GetActorLocation() - OutStartTrace) | AimDir);
	}

	return OutStartTrace;
}


FHitResult ASWeapon::WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const
{
	FCollisionQueryParams TraceParams(TEXT("WeaponTrace"), true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingle(Hit, TraceFrom, TraceTo, COLLISION_WEAPON, TraceParams);

	return Hit;
}



void ASWeapon::HandleFiring()
{
	if (CanFire())
	{
		if (GetNetMode() != NM_DedicatedServer)
		{
			SimulateWeaponFire();
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			FireWeapon();

			// TODO: Consume Ammo

			// Update firing FX on remote clients if this is called on server
			BurstCounter++;
		}
	}
	// TODO: End repeat fire if ammo runs dry

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		if (Role < ROLE_Authority)
		{
			ServerHandleFiring();
		}

		// TODO: if weapon is automatic firing rifle -> Setup refiring timer. (calls HandleFiring()

		bRefiring = (CurrentState == EWeaponState::Firing && TimeBetweenShots > 0.0f);
		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(HandleFiringTimerHandle, this, &ASWeapon::HandleFiring, TimeBetweenShots, false);
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}


void ASWeapon::SimulateWeaponFire()
{
	if (MuzzleFX)
	{
		MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh, MuzzleAttachPoint);	
	}

	if (!bPlayingFireAnim)
	{
		PlayWeaponAnimation(FireAnim);
		bPlayingFireAnim = true;
	}

	PlayWeaponSound(FireSound);

	// 	ASPlayerController* PC = (MyPawn != nullptr) ? Cast<ASPlayerController>(MyPawn->Controller) : nullptr;
	// 	if (PC && PC->IsLocalController())
	// 	{
	// 		// TODO: Add camera roll oscillation
	// 	}
}


void ASWeapon::StopSimulatingWeaponFire()
{
	if (bPlayingFireAnim)
	{
		StopWeaponAnimation(FireAnim);
		bPlayingFireAnim = false;
	}
}



void ASWeapon::OnRep_BurstCounter()
{
	if (BurstCounter > 0)
	{
		SimulateWeaponFire();
	}
	else
	{
		StopSimulatingWeaponFire();
	}
}



bool ASWeapon::ServerHandleFiring_Validate()
{
	return true;
}


void ASWeapon::ServerHandleFiring_Implementation()
{
	const bool bShouldUpdateAmmo = CanFire();

	HandleFiring();

	if (bShouldUpdateAmmo)
	{
		// Update firing FX on remote clients
		BurstCounter++;
	}
}


void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASWeapon, MyPawn);
	DOREPLIFETIME_CONDITION(ASWeapon, BurstCounter, COND_SkipOwner);
}


FVector ASWeapon::GetMuzzleLocation() const
{
	return Mesh->GetSocketLocation(MuzzleAttachPoint);
}


FVector ASWeapon::GetMuzzleDirection() const
{
	return Mesh->GetSocketRotation(MuzzleAttachPoint).Vector();
}


UAudioComponent* ASWeapon::PlayWeaponSound(USoundCue* SoundToPlay)
{
	UAudioComponent* AC = nullptr;
	if (SoundToPlay && MyPawn)
	{
		AC = UGameplayStatics::PlaySoundAttached(SoundToPlay, MyPawn->GetRootComponent());
	}

	return AC;
}


EWeaponState ASWeapon::GetCurrentState() const
{
	return CurrentState;
}


void ASWeapon::SetWeaponState(EWeaponState NewState)
{
	const EWeaponState PrevState = CurrentState;

	if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing)
	{
		OnBurstFinished();
	}

	CurrentState = NewState;

	if (PrevState != EWeaponState::Firing && NewState == EWeaponState::Firing)
	{
		OnBurstStarted();
	}
}


void ASWeapon::OnBurstStarted()
{
	// Start firing, can be delayed to satisfy TimeBetweenShots
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && TimeBetweenShots > 0.0f &&
		LastFireTime + TimeBetweenShots > GameTime)
	{
		GetWorldTimerManager().SetTimer(HandleFiringTimerHandle, this, &ASWeapon::HandleFiring, LastFireTime + TimeBetweenShots - GameTime, false);
	}
	else
	{
		HandleFiring();
	}
}


void ASWeapon::OnBurstFinished()
{
	BurstCounter = 0;

	if (GetNetMode() != NM_DedicatedServer)
	{
		StopSimulatingWeaponFire();
	}

	GetWorldTimerManager().ClearTimer(HandleFiringTimerHandle);
	bRefiring = false;
}


void ASWeapon::DetermineWeaponState()
{
	EWeaponState NewState = EWeaponState::Idle;

	if (bIsEquipped)
	{
		if (bWantsToFire && CanFire())
		{
			NewState = EWeaponState::Firing;
		}
	}
	else if (bPendingEquip)
	{
		NewState = EWeaponState::Equipping;
	}

	SetWeaponState(NewState);
}


float ASWeapon::GetEquipStartedTime() const
{
	return EquipStartedTime;
}


float ASWeapon::GetEquipDuration() const
{
	return EquipDuration;
}


float ASWeapon::PlayWeaponAnimation(UAnimMontage* Animation, float InPlayRate, FName StartSectionName)
{
	float Duration = 0.0f;
	if (MyPawn)
	{
		if (Animation)
		{
			Duration = MyPawn->PlayAnimMontage(Animation, InPlayRate, StartSectionName);
		}
	}

	return Duration;
}


void ASWeapon::StopWeaponAnimation(UAnimMontage* Animation)
{
	if (MyPawn)
	{
		if (Animation)
		{
			MyPawn->StopAnimMontage(Animation);
		}
	}
}


void ASWeapon::OnEquipFinished()
{
	AttachMeshToPawn();

	bIsEquipped = true;
	bPendingEquip = false;

	DetermineWeaponState();
}


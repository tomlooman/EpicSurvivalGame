// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SCharacter.h"
#include "SWeapon.generated.h"

UENUM()
enum class EWeaponState
{
	Idle,
	Firing,
	Equipping,
	Reloading
};

/**
 * 
 */
UCLASS(ABSTRACT, Blueprintable)
class SURVIVALGAME_API ASWeapon : public AActor
{
	GENERATED_UCLASS_BODY()

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void OnEquip();

	virtual void OnUnEquip();

	virtual void OnEquipFinished();

	virtual void OnEnterInventory(ASCharacter* NewOwner);

	virtual void OnLeaveInventory();

	bool IsEquipped() const;

	bool IsAttachedToPawn() const;

	/** get weapon mesh (needs pawn owner to determine variant) */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	USkeletalMeshComponent* GetWeaponMesh() const;

	/** get pawn owner */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	class ASCharacter* GetPawnOwner() const;

	/** set the weapon's owning pawn */
	void SetOwningPawn(ASCharacter* NewOwner);

	float GetEquipStartedTime() const;

	float GetEquipDuration() const;

	/** last time when this weapon was switched to */
	float EquipStartedTime;

	/** how much time weapon needs to be equipped */
	float EquipDuration;

	bool bIsEquipped;

	bool bPendingEquip;

	FTimerHandle HandleFiringTimerHandle;

	FTimerHandle EquipFinishedTimerHandle;

	/* The class to spawn in the level when dropped */
	UPROPERTY(EditDefaultsOnly, Category = "Game|Weapon")
	TSubclassOf<class ASWeaponPickup> WeaponPickupClass;

	/* The character socket to store this item at. */
	EInventorySlot StorageSlot;

protected:

	/** pawn owner */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
	class ASCharacter* MyPawn;

	/** weapon mesh: 3rd person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh;

	UFUNCTION()
	void OnRep_MyPawn();

	/* You can assign default values to function parameters, these are then optional to specify/override when calling the function. */
	void AttachMeshToPawn(EInventorySlot Slot = EInventorySlot::Hands);

	/** detaches weapon mesh from pawn */
	void DetachMeshFromPawn();

public:

	FORCEINLINE EInventorySlot GetStorageSlot()
	{
		return StorageSlot;
	}

	/************************************************************************/
	/* Fire & Damage Handling                                               */
	/************************************************************************/

public:

	void StartFire();

	void StopFire();

	EWeaponState GetCurrentState() const;

protected:

	bool CanFire() const;

	FVector GetAdjustedAim() const;

	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;

	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

	/* With PURE_VIRTUAL we skip implementing the function in SWeapon.cpp and can do this in SWeaponInstant.cpp / SFlashlight.cpp instead */
	virtual void FireWeapon() PURE_VIRTUAL(ASWeapon::FireWeapon, );

	UPROPERTY(EditDefaultsOnly)
	float TimeBetweenShots;

private:

	void SetWeaponState(EWeaponState NewState);

	void DetermineWeaponState();

	virtual void HandleFiring();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStartFire();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStopFire();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerHandleFiring();

	void OnBurstStarted();

	void OnBurstFinished();

	bool bWantsToFire;

	EWeaponState CurrentState;

	bool bRefiring;

	float LastFireTime;

	/************************************************************************/
	/* Simulation & FX                                                      */
	/************************************************************************/

private:

	UFUNCTION()
	void OnRep_BurstCounter();

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* EquipSound;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* MuzzleFX;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* EquipAnim;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* FireAnim;

	UPROPERTY(Transient)
	UParticleSystemComponent* MuzzlePSC;

	UPROPERTY(EditDefaultsOnly)
	FName MuzzleAttachPoint;

	bool bPlayingFireAnim;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_BurstCounter)
	int32 BurstCounter;

protected:

	virtual void SimulateWeaponFire();

	virtual void StopSimulatingWeaponFire();

	FVector GetMuzzleLocation() const;

	FVector GetMuzzleDirection() const;

	UAudioComponent* PlayWeaponSound(USoundCue* SoundToPlay);

	float PlayWeaponAnimation(UAnimMontage* Animation, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	void StopWeaponAnimation(UAnimMontage* Animation);
};

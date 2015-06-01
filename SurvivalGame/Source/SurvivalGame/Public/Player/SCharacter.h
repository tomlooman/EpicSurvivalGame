// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SBaseCharacter.h"
#include "SCharacter.generated.h"

UCLASS()
class SURVIVALGAME_API ASCharacter : public ASBaseCharacter
{
	GENERATED_BODY()

	ASCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	/* Called every frame */
	virtual void Tick(float DeltaSeconds) override;

	/* Called to bind functionality to input */
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PawnClientRestart() override;

	/* Stop playing all montages */
	void StopAllAnimMontages();

	UFUNCTION(BlueprintCallable, Category = "AI")
	float GetLastNoiseLoudness();

	UFUNCTION(BlueprintCallable, Category = "AI")
	float GetLastMakeNoiseTime();

	float LastNoiseLoudness;

	float LastMakeNoiseTime;

private:

	/* Boom to handle distance to player mesh. */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* CameraBoomComp;

	/* Primary camera of the player*/
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USCarryObjectComponent* CarriedObjectComp;

public:

	FORCEINLINE UCameraComponent* GetCameraComponent()
	{
		return CameraComp;
	}

	/* MakeNoise hook to trigger AI noise emitting (Loudness between 0.0-1.0)  */
	UFUNCTION(BlueprintCallable, Category = "AI")
	void MakePawnNoise(float Loudness);

	/************************************************************************/
	/* Movement                                                             */
	/************************************************************************/

	virtual void MoveForward(float Val);

	virtual void MoveRight(float Val);

	/* Client mapped to Input */
	void OnCrouchToggle();

	/* Client mapped to Input */
	void OnStartJump();

	/* Client mapped to Input */
	void OnStopJump();

	/* Client mapped to Input */
	void OnStartSprinting();

	/* Client mapped to Input */
	void OnStopSprinting();

	/* Character wants to run, checked during Tick to see if allowed */
	UPROPERTY(Transient, Replicated)
	bool bWantsToRun;

	/* Is character currently performing a jump action. Resets on landed.  */
	UPROPERTY(Transient, Replicated)
	bool bIsJumping;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsInitiatedJump() const;

	void SetIsJumping(bool NewJumping);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetIsJumping(bool NewJumping);

	void ServerSetIsJumping_Implementation(bool NewJumping);

	bool ServerSetIsJumping_Validate(bool NewJumping);

	void OnLanded(const FHitResult& Hit) override;

	/* Client/local call to update sprint state  */
	void SetSprinting(bool NewSprinting);

	/* Server side call to update actual sprint state */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetSprinting(bool NewSprinting);

	void ServerSetSprinting_Implementation(bool NewSprinting);

	bool ServerSetSprinting_Validate(bool NewSprinting);

	UFUNCTION(BlueprintCallable, Category = Movement)
	bool IsSprinting() const;

	float GetSprintingSpeedModifier() const;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintingSpeedModifier;

	/************************************************************************/
	/* Object Interaction                                                   */
	/************************************************************************/

	/* Input mapped function for carry object component */
	void OnToggleCarryActor();

	/* Use the usable actor currently in focus, if any */
	virtual void Use();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUse();

	void ServerUse_Implementation();

	bool ServerUse_Validate();

	class ASUsableActor* GetUsableInView();

	/*Max distance to use/focus on actors. */
	UPROPERTY(EditDefaultsOnly, Category = "ObjectInteraction")
	float MaxUseDistance;

	/* True only in first frame when focused on a new usable actor. */
	bool bHasNewFocus;

	class ASUsableActor* FocusedUsableActor;

	/************************************************************************/
	/* Targeting                                                            */
	/************************************************************************/

	void OnStartTargeting();

	void OnEndTargeting();

	void SetTargeting(bool NewTargeting);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetTargeting(bool NewTargeting);

	void ServerSetTargeting_Implementation(bool NewTargeting);

	bool ServerSetTargeting_Validate(bool NewTargeting);
	
	/* Is player aiming down sights */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	bool IsTargeting() const;

	float GetTargetingSpeedModifier() const;

	/* Retrieve Pitch/Yaw from current camera */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	FRotator GetAimOffsets() const;

	UPROPERTY(Transient, Replicated)
	bool bIsTargeting;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingSpeedModifier;

	/************************************************************************/
	/* Hunger                                                               */
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetHunger() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetMaxHunger() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	void ConsumeFood(float AmountRestored);

	/* Increments hunger, used by timer. */
	void IncrementHunger();

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float IncrementHungerInterval;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float IncrementHungerAmount;

	/* Limit when player suffers Hitpoints from extreme hunger */
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float CriticalHungerThreshold;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition", Replicated)
	float Hunger;

	// Documentation Note: MaxHunger does not need to be replicated, only values that change and are displayed or used by clients should ever be replicated.
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float MaxHunger;
	
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float HungerDamagePerInterval;

	/* Damage type applied when player suffers critical hunger */
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	TSubclassOf<UDamageType> HungerDamageType;

	/************************************************************************/
	/* Damage & Death                                                       */
	/************************************************************************/

	virtual void OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser) override;

	virtual void Suicide();

	virtual void KilledBy(class APawn* EventInstigator);

	/************************************************************************/
	/* Weapons & Inventory                                                  */
	/************************************************************************/

private:

	/* Attachpoint for active weapon/item in hands */
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName WeaponAttachPoint;

	/* Attachpoint for items carried on the belt/pelvis. */
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName PelvisAttachPoint;

	/* Attachpoint for primary weapons */
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName SpineAttachPoint;

	bool bWantsToFire;

	/* Distance away from character when dropping inventory items. */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	float DropItemDistance;

	void OnReload();

	/* Mapped to input */
	void OnStartFire();

	/* Mapped to input */
	void OnStopFire();

	/* Mapped to input */
	void OnNextWeapon();

	/* Mapped to input */
	void OnPrevWeapon();

	/* Mapped to input */
	void OnEquipPrimaryWeapon();

	/* Mapped to input */
	void OnEquipSecondaryWeapon();

	void StartWeaponFire();

	void StopWeaponFire();

	void DestroyInventory();

	/* Mapped to input. Drops current weapon */
	void DropWeapon();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerDropWeapon();

	void ServerDropWeapon_Implementation();

	bool ServerDropWeapon_Validate();

public:

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	ASWeapon* GetCurrentWeapon() const;

	/* Check if the specified slot is available, limited to one item per type (primary, secondary) */
	bool WeaponSlotAvailable(EInventorySlot CheckSlot);

	/* Check if pawn is allowed to fire weapon */
	bool CanFire() const;

	bool CanReload() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsFiring() const;

	/* Return socket name for attachments (to match the socket in the character skeleton) */
	FName GetInventoryAttachPoint(EInventorySlot Slot) const;

	/* All weapons/items the player currently holds */
	UPROPERTY(Transient, Replicated)
	TArray<ASWeapon*> Inventory;

	void SpawnDefaultInventory();

	void SetCurrentWeapon(class ASWeapon* newWeapon, class ASWeapon* LastWeapon = nullptr);

	void EquipWeapon(ASWeapon* Weapon);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerEquipWeapon(ASWeapon* Weapon);

	void ServerEquipWeapon_Implementation(ASWeapon* Weapon);

	bool ServerEquipWeapon_Validate(ASWeapon* Weapon);

	/* OnRep functions can use a parameter to hold the previous value of the variable. Very useful when you need to handle UnEquip etc. */
	UFUNCTION()
	void OnRep_CurrentWeapon(ASWeapon* LastWeapon);

	void AddWeapon(class ASWeapon* Weapon);

	void RemoveWeapon(class ASWeapon* Weapon);

	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	class ASWeapon* CurrentWeapon;

	class ASWeapon* PreviousWeapon;

	/* The default weapons to spawn with */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TArray<TSubclassOf<class ASWeapon>> DefaultInventoryClasses;


	/* Update the weapon mesh to the newly equipped weapon, this is triggered during an anim montage.
		NOTE: Requires an AnimNotify created in the Equip animation to tell us when to swap the meshes. */
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SwapToNewWeaponMesh();
};

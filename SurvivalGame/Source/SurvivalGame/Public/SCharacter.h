// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

UCLASS()
class SURVIVALGAME_API ASCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

private:

	/* Boom to handle distance to player mesh. */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* CameraBoomComp;

	/* Primary camera of the player*/
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* CameraComp;

public:

	virtual void PostInitializeComponents() override;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

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

	void OnLanded(const FHitResult& Hit) override;

	/* Client/local call to update sprint state  */
	void SetSprinting(bool NewSprinting);

	/* Server side call to update actual sprint state */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetSprinting(bool NewSprinting);

	UFUNCTION(BlueprintCallable, Category = Movement)
	bool IsSprinting() const;

	float GetSprintingSpeedModifier() const;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintingSpeedModifier;

	/************************************************************************/
	/* Object Interaction                                                   */
	/************************************************************************/

	/* Use the usable actor currently in focus, if any */
	virtual void Use();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUse();

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
	/* Hitpoints & Hunger                                                   */
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetHunger() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetMaxHunger() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	void ConsumeFood(float AmountRestored);

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	bool IsAlive() const;

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
	float Health;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition", Replicated)
	float Hunger;

	// Documentation Note: MaxHunger does not need to be replicated, only values that change and are displayed or used by clients should ever be replicated.
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float MaxHunger;

	/************************************************************************/
	/* Damage, Hit & Death                                                  */
	/************************************************************************/

	/* Take damage & handle death */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

};

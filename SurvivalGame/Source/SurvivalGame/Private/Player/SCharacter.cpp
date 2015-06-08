// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SCharacter.h"
#include "SUsableActor.h"
#include "SHUD.h"
#include "SWeapon.h"
#include "SWeaponPickup.h"
#include "SCharacterMovementComponent.h"
#include "SCarryObjectComponent.h"
#include "SBaseCharacter.h"


// Sets default values
ASCharacter::ASCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	// Adjust jump to make it less floaty
	MoveComp->GravityScale = 1.5f;
	MoveComp->JumpZVelocity = 620;
	MoveComp->bCanWalkOffLedgesWhenCrouching = true;
	MoveComp->MaxWalkSpeedCrouched = 200;

	/* Ignore this channel or it will absorb the trace impacts instead of the skeletal mesh */
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	// Enable crouching
	MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;

	CameraBoomComp = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoomComp->SocketOffset = FVector(0, 35, 0);
	CameraBoomComp->TargetOffset = FVector(0, 0, 55);
	CameraBoomComp->bUsePawnControlRotation = true;
	CameraBoomComp->AttachParent = GetRootComponent();

	CameraComp = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera"));
	CameraComp->AttachParent = CameraBoomComp;

	CarriedObjectComp = ObjectInitializer.CreateDefaultSubobject<USCarryObjectComponent>(this, TEXT("CarriedObjectComp"));
	CarriedObjectComp->AttachParent = GetRootComponent();

	MaxUseDistance = 500;
	DropItemDistance = 100;
	bHasNewFocus = true;
	TargetingSpeedModifier = 0.5f;
	SprintingSpeedModifier = 2.5f;

	Health = 100;

	IncrementHungerAmount = 0.5f;
	IncrementHungerInterval = 5.0f;
	CriticalHungerThreshold = 90;
	HungerDamagePerInterval = 1.0f;
	MaxHunger = 100;
	Hunger = 0;

	/* Names as specified in the character skeleton */
	WeaponAttachPoint = TEXT("WeaponSocket");
	PelvisAttachPoint = TEXT("PelvisSocket");
	SpineAttachPoint = TEXT("SpineSocket");
}


void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		// Set a timer to increment hunger every interval
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &ASCharacter::IncrementHunger, IncrementHungerInterval, true);

		SpawnDefaultInventory();
	}
}


void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWantsToRun && !IsSprinting())
	{
		SetSprinting(true);
	}

	if (Controller && Controller->IsLocalController())
	{
		ASUsableActor* Usable = GetUsableInView();

		// End Focus
		if (FocusedUsableActor != Usable)
		{
			if (FocusedUsableActor)
			{
				FocusedUsableActor->OnEndFocus();
			}

			bHasNewFocus = true;
		}

		// Assign new Focus
		FocusedUsableActor = Usable;

		// Start Focus.
		if (Usable)
		{
			if (bHasNewFocus)
			{
				Usable->OnBeginFocus();
				bHasNewFocus = false;
			}
		}
	}
}

void ASCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	DestroyInventory();
}


// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	// Movement
	InputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	InputComponent->BindAction("SprintHold", IE_Pressed, this, &ASCharacter::OnStartSprinting);
	InputComponent->BindAction("SprintHold", IE_Released, this, &ASCharacter::OnStopSprinting);

	InputComponent->BindAction("CrouchToggle", IE_Released, this, &ASCharacter::OnCrouchToggle);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::OnStartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::OnStopJump);

	// Interaction
	InputComponent->BindAction("Use", IE_Pressed, this, &ASCharacter::Use);
	InputComponent->BindAction("DropWeapon", IE_Pressed, this, &ASCharacter::DropWeapon);

	// Weapons
	InputComponent->BindAction("Targeting", IE_Pressed, this, &ASCharacter::OnStartTargeting);
	InputComponent->BindAction("Targeting", IE_Released, this, &ASCharacter::OnEndTargeting);

	InputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::OnStartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::OnStopFire);

	InputComponent->BindAction("Reload", IE_Pressed, this, &ASCharacter::OnReload);

	InputComponent->BindAction("NextWeapon", IE_Pressed, this, &ASCharacter::OnNextWeapon);
	InputComponent->BindAction("PrevWeapon", IE_Pressed, this, &ASCharacter::OnPrevWeapon);

	InputComponent->BindAction("EquipPrimaryWeapon", IE_Pressed, this, &ASCharacter::OnEquipPrimaryWeapon);
	InputComponent->BindAction("EquipSecondaryWeapon", IE_Pressed, this, &ASCharacter::OnEquipSecondaryWeapon);

	/* Input binding for the carry object component */
	InputComponent->BindAction("PickupObject", IE_Pressed, this, &ASCharacter::OnToggleCarryActor);
}


void ASCharacter::MoveForward(float Val)
{
	if (Controller && Val != 0.f)
	{
		// Limit pitch when walking or falling
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());
		const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);

		AddMovementInput(Direction, Val);
	}
}


void ASCharacter::MoveRight(float Val)
{
	if (Val != 0.f)
	{
		const FRotator Rotation = GetActorRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Val);
	}
}


/*
Performs ray-trace to find closest looked-at UsableActor.
*/
ASUsableActor* ASCharacter::GetUsableInView()
{
	FVector CamLoc;
	FRotator CamRot;

	if (Controller == nullptr)
		return nullptr;

	Controller->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector TraceStart = CamLoc;
	const FVector Direction = CamRot.Vector();
	const FVector TraceEnd = TraceStart + (Direction * MaxUseDistance);

	FCollisionQueryParams TraceParams(TEXT("TraceUsableActor"), true, this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;

	/* Not tracing complex uses the rough collision instead making tiny objects easier to select. */
	TraceParams.bTraceComplex = false;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingle(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f);

	return Cast<ASUsableActor>(Hit.GetActor());
}


void ASCharacter::Use()
{
	// Only allow on server. If called on client push this request to the server
	if (Role == ROLE_Authority)
	{
		ASUsableActor* Usable = GetUsableInView();
		if (Usable)
		{
			Usable->OnUsed(this);
		}
	}
	else
	{
		ServerUse();
	}
}


void ASCharacter::ServerUse_Implementation()
{
	Use();
}


bool ASCharacter::ServerUse_Validate()
{
	return true;
}


void ASCharacter::OnStartTargeting()
{
	if (CarriedObjectComp->GetIsCarryingActor())
	{
		CarriedObjectComp->Drop();
	}

	SetTargeting(true);
}


void ASCharacter::OnEndTargeting()
{
	SetTargeting(false);
}


void ASCharacter::SetTargeting(bool NewTargeting)
{
	bIsTargeting = NewTargeting;

	if (Role < ROLE_Authority)
	{
		ServerSetTargeting(NewTargeting);
	}
}


void ASCharacter::ServerSetTargeting_Implementation(bool NewTargeting)
{
	SetTargeting(NewTargeting);
}


bool ASCharacter::ServerSetTargeting_Validate(bool NewTargeting)
{
	return true;
}



bool ASCharacter::IsTargeting() const
{
	return bIsTargeting;
}


float ASCharacter::GetTargetingSpeedModifier() const
{
	return TargetingSpeedModifier;
}


void ASCharacter::OnStartJump()
{
	bPressedJump = true;

	SetIsJumping(true);
}


void ASCharacter::OnStopJump()
{
	bPressedJump = false;
}


bool ASCharacter::IsInitiatedJump() const
{
	return bIsJumping;
}


void ASCharacter::SetIsJumping(bool NewJumping)
{
	// Go to standing pose if trying to jump while crouched
	if (bIsCrouched && NewJumping)
	{
		UnCrouch();
	}
	else
	{
		bIsJumping = NewJumping;
	}

	if (Role < ROLE_Authority)
	{
		ServerSetIsJumping(NewJumping);
	}
}


void ASCharacter::OnLanded(const FHitResult& Hit)
{
	Super::OnLanded(Hit);

	SetIsJumping(false);
}


void ASCharacter::ServerSetIsJumping_Implementation(bool NewJumping)
{
	SetIsJumping(NewJumping);
}


bool ASCharacter::ServerSetIsJumping_Validate(bool NewJumping)
{
	return true;
}


void ASCharacter::OnStartSprinting()
{
	if (CarriedObjectComp->GetIsCarryingActor())
	{
		CarriedObjectComp->Drop();
	}

	SetSprinting(true);
}


void ASCharacter::OnStopSprinting()
{
	SetSprinting(false);
}


void ASCharacter::SetSprinting(bool NewSprinting)
{
	bWantsToRun = NewSprinting;

	if (bIsCrouched)
	{
		UnCrouch();
	}

	if (bWantsToRun)
	{
		StopWeaponFire();
	}

	if (Role < ROLE_Authority)
	{
		ServerSetSprinting(NewSprinting);
	}
}


void ASCharacter::ServerSetSprinting_Implementation(bool NewSprinting)
{
	SetSprinting(NewSprinting);
}


bool ASCharacter::ServerSetSprinting_Validate(bool NewSprinting)
{
	return true;
}


bool ASCharacter::IsSprinting() const
{
	if (!GetCharacterMovement())
		return false;

	return bWantsToRun && !IsTargeting() && !GetVelocity().IsZero() 
		// Don't allow sprint while strafing sideways or standing still (1.0 is straight forward, -1.0 is backward while near 0 is sideways or standing still)
		&& (FVector::DotProduct(GetVelocity().GetSafeNormal2D(), GetActorRotation().Vector()) > 0.8); // Changing this value to 0.1 allows for diagonal sprinting. (holding W+A or W+D keys)
}


float ASCharacter::GetSprintingSpeedModifier() const
{
	return SprintingSpeedModifier;
}


void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Value is already updated locally, skip in replication step
	DOREPLIFETIME_CONDITION(ASCharacter, bWantsToRun, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ASCharacter, bIsTargeting, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ASCharacter, bIsJumping, COND_SkipOwner);

	// Replicate to every client, no special condition required
	DOREPLIFETIME(ASCharacter, Health);
	DOREPLIFETIME(ASCharacter, Hunger);

	DOREPLIFETIME(ASCharacter, LastTakeHitInfo);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, Inventory);
	/* If we did not display the current inventory on the player mesh we could optimize replication by using this replication condition. */
	/* DOREPLIFETIME_CONDITION(ASCharacter, Inventory, COND_OwnerOnly);*/
}

void ASCharacter::OnCrouchToggle()
{
	if (IsSprinting())
	{
		SetSprinting(false);
	}

	// If we are crouching then CanCrouch will return false. If we cannot crouch then calling Crouch() wont do anything
	if (CanCrouch())
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}


FRotator ASCharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}


float ASCharacter::GetHunger() const
{
	return Hunger;
}


float ASCharacter::GetMaxHunger() const
{
	return MaxHunger;
}


void ASCharacter::ConsumeFood(float AmountRestored)
{
	// Reduce Hunger, ensure we do not go outside of our bounds
	Hunger = FMath::Clamp(Hunger - AmountRestored, 0.0f, GetMaxHunger());

	// Restore Hitpoints
	Health = FMath::Clamp(Health + AmountRestored, 0.0f, GetMaxHealth());

	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC)
	{
		ASHUD* MyHUD = Cast<ASHUD>(PC->GetHUD());
		if (MyHUD)
		{
			MyHUD->MessageReceived("Food consumed!");
		}
	}
}


void ASCharacter::IncrementHunger()
{
	Hunger = FMath::Clamp(Hunger + IncrementHungerAmount, 0.0f, GetMaxHunger());

	if (Hunger > CriticalHungerThreshold)
	{
		FDamageEvent DmgEvent;
		DmgEvent.DamageTypeClass = HungerDamageType;

		// Apply damage to self.
		TakeDamage(HungerDamagePerInterval, DmgEvent, GetController(), this);
	}
}


void ASCharacter::OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser)
{
	if (bIsDying)
	{
		return;
	}

	DestroyInventory();
	StopAllAnimMontages();

	Super::OnDeath(KillingDamage, DamageEvent, PawnInstigator, DamageCauser);
}


bool ASCharacter::CanFire() const
{
	/* Add your own checks here, for example non-shooting areas or checking if player is in an NPC dialogue etc. */
	return IsAlive();
}


bool ASCharacter::CanReload() const
{
	return IsAlive();
}


bool ASCharacter::IsFiring() const
{
	return CurrentWeapon && CurrentWeapon->GetCurrentState() == EWeaponState::Firing;
}


FName ASCharacter::GetInventoryAttachPoint(EInventorySlot Slot) const
{
	/* Return the socket name for the specified storage slot */
	switch (Slot)
	{
	case EInventorySlot::Hands:
		return WeaponAttachPoint;
	case EInventorySlot::Primary:
		return SpineAttachPoint;
	case EInventorySlot::Secondary:
		return PelvisAttachPoint;
	default:
		// Not implemented.
		return "";
	}
}


void ASCharacter::SpawnDefaultInventory()
{
	if (Role < ROLE_Authority)
	{	
		return;
	}

	for (int32 i = 0; i < DefaultInventoryClasses.Num(); i++)
	{
		if (DefaultInventoryClasses[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.bNoCollisionFail = true;
			ASWeapon* NewWeapon = GetWorld()->SpawnActor<ASWeapon>(DefaultInventoryClasses[i], SpawnInfo);

			AddWeapon(NewWeapon);
		}
	}
}


void ASCharacter::DestroyInventory()
{
	if (Role < ROLE_Authority)
	{	
		return;
	}

	for (int32 i = Inventory.Num() - 1; i >= 0; i--)
	{
		ASWeapon* Weapon = Inventory[i];
		if (Weapon)
		{
			RemoveWeapon(Weapon);
			Weapon->Destroy();
		}
	}
}


void ASCharacter::SetCurrentWeapon(class ASWeapon* NewWeapon, class ASWeapon* LastWeapon)
{
	/* Maintain a reference for visual weapon swapping */
	PreviousWeapon = LastWeapon;

	ASWeapon* LocalLastWeapon = nullptr;
	if (LastWeapon)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	// UnEquip the current
	bool bHasPreviousWeapon = false;
	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
		bHasPreviousWeapon = true;
	}

	CurrentWeapon = NewWeapon;

	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);
		/* Only play equip animation when we already hold an item in hands */
		NewWeapon->OnEquip(bHasPreviousWeapon);
	}

	/* NOTE: If you don't have an equip animation w/ animnotify to swap the meshes halfway through, then uncomment this to immediately swap instead */
	//SwapToNewWeaponMesh();
}


void ASCharacter::OnRep_CurrentWeapon(ASWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}


ASWeapon* ASCharacter::GetCurrentWeapon() const
{
	return CurrentWeapon;
}


void ASCharacter::EquipWeapon(ASWeapon* Weapon)
{
	if (Weapon)
	{
		/* Ignore if trying to equip already equipped weapon */
		if (Weapon == CurrentWeapon)
			return;

		if (Role == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon, CurrentWeapon);
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}
}


bool ASCharacter::ServerEquipWeapon_Validate(ASWeapon* Weapon)
{
	return true;
}


void ASCharacter::ServerEquipWeapon_Implementation(ASWeapon* Weapon)
{
	EquipWeapon(Weapon);
}


void ASCharacter::AddWeapon(class ASWeapon* Weapon)
{
	if (Weapon && Role == ROLE_Authority)
	{
		Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);

		// Equip first weapon in inventory
		if (Inventory.Num() > 0 && CurrentWeapon == nullptr)
		{
			EquipWeapon(Inventory[0]);
		}
	}
}


void ASCharacter::RemoveWeapon(class ASWeapon* Weapon)
{
	if (Weapon && Role == ROLE_Authority)
	{
		bool bIsCurrent = CurrentWeapon == Weapon;

		if (Inventory.Contains(Weapon))
		{
			Weapon->OnLeaveInventory();
		}
		Inventory.RemoveSingle(Weapon);

		/* Replace weapon if we removed our current weapon */
		if (bIsCurrent && Inventory.Num() > 0)
			SetCurrentWeapon(Inventory[0]);

		/* Clear reference to weapon if we have no items left in inventory */
		if (Inventory.Num() == 0)
			SetCurrentWeapon(nullptr);
	}
}


void ASCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	/* Equip the weapon on the client side. */
	SetCurrentWeapon(CurrentWeapon);
}


void ASCharacter::OnReload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartReload();
	}
}


void ASCharacter::OnStartFire()
{
	if (IsSprinting())
	{
		SetSprinting(false);
	}

	if (CarriedObjectComp->GetIsCarryingActor())
	{
		StopWeaponFire();

		CarriedObjectComp->Throw();
		return;
	}

	StartWeaponFire();
}


void ASCharacter::OnStopFire()
{
	StopWeaponFire();
}


void ASCharacter::StartWeaponFire()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		if (CurrentWeapon)
		{
			CurrentWeapon->StartFire();
		}
	}
}


void ASCharacter::StopWeaponFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
	}
}


void ASCharacter::OnNextWeapon()
{
	if (CarriedObjectComp->GetIsCarryingActor())
	{
		CarriedObjectComp->Rotate(1.0f);
		return;
	}

	if (Inventory.Num() >= 2) // TODO: Check for weaponstate.
	{
		const int32 CurrentWeaponIndex = Inventory.IndexOfByKey(CurrentWeapon);
		ASWeapon* NextWeapon = Inventory[(CurrentWeaponIndex + 1) % Inventory.Num()];
		EquipWeapon(NextWeapon);
	}
}


void ASCharacter::OnPrevWeapon()
{
	if (CarriedObjectComp->GetIsCarryingActor())
	{
		CarriedObjectComp->Rotate(-1.0f);
		return;
	}

	if (Inventory.Num() >= 2) // TODO: Check for weaponstate.
	{
		const int32 CurrentWeaponIndex = Inventory.IndexOfByKey(CurrentWeapon);
		ASWeapon* PrevWeapon = Inventory[(CurrentWeaponIndex - 1 + Inventory.Num()) % Inventory.Num()];
		EquipWeapon(PrevWeapon);
	}
}


void ASCharacter::DropWeapon()
{
	if (Role < ROLE_Authority)
	{
		ServerDropWeapon();
		return;
	}

	if (CurrentWeapon)
	{
		FVector CamLoc;
		FRotator CamRot;

		if (Controller == nullptr)
			return;

		/* Find a location to drop the item, slightly in front of the player. */
		Controller->GetPlayerViewPoint(CamLoc, CamRot);
		const FVector Direction = CamRot.Vector();
		const FVector SpawnLocation = GetActorLocation() + (Direction * DropItemDistance);

		FActorSpawnParameters SpawnInfo;
		SpawnInfo.bNoCollisionFail = true;
		ASWeaponPickup* NewWeaponPickup = GetWorld()->SpawnActor<ASWeaponPickup>(CurrentWeapon->WeaponPickupClass, SpawnLocation, FRotator::ZeroRotator, SpawnInfo);

		if (NewWeaponPickup)
		{
			/* Apply torque to make it spin when dropped. */
			UStaticMeshComponent* MeshComp = NewWeaponPickup->GetMeshComponent();
			if (MeshComp)
			{
				MeshComp->SetSimulatePhysics(true);
				MeshComp->AddTorque(FVector(1, 1, 1) * 4000000);
			}
		}

		RemoveWeapon(CurrentWeapon);
	}
}


void ASCharacter::ServerDropWeapon_Implementation()
{
	DropWeapon();
}


bool ASCharacter::ServerDropWeapon_Validate()
{
	return true;
}


void ASCharacter::OnEquipPrimaryWeapon()
{
	if (Inventory.Num() >= 1)
	{
		/* Find first weapon that uses primary slot. */
		for (int32 i = 0; i < Inventory.Num(); i++)
		{
			ASWeapon* Weapon = Inventory[i];
			if (Weapon->GetStorageSlot() == EInventorySlot::Primary)
			{
				EquipWeapon(Weapon);
			}
		}
	}
}


void ASCharacter::OnEquipSecondaryWeapon()
{
	if (Inventory.Num() >= 2)
	{
		/* Find first weapon that uses secondary slot. */
		for (int32 i = 0; i < Inventory.Num(); i++)
		{
			ASWeapon* Weapon = Inventory[i];
			if (Weapon->GetStorageSlot() == EInventorySlot::Secondary)
			{
				EquipWeapon(Weapon);
			}
		}
	}
}

bool ASCharacter::WeaponSlotAvailable(EInventorySlot CheckSlot)
{
	/* Iterate all weapons to see if requested slot is occupied */
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		ASWeapon* Weapon = Inventory[i];
		if (Weapon)
		{
			if (Weapon->GetStorageSlot() == CheckSlot)
				return false;
		}
	}

	return true;

	/* Special find function as alternative to looping the array and performing if statements 
		the [=] prefix means "capture by value", other options include [] "capture nothing" and [&] "capture by reference" */
	//return nullptr == Inventory.FindByPredicate([=](ASWeapon* W){ return W->GetStorageSlot() == CheckSlot; });
}


void ASCharacter::StopAllAnimMontages()
{
	USkeletalMeshComponent* UseMesh = GetMesh();
	if (UseMesh && UseMesh->AnimScriptInstance)
	{
		UseMesh->AnimScriptInstance->Montage_Stop(0.0f);
	}
}


void ASCharacter::MakePawnNoise(float Loudness)
{
	if (Role == ROLE_Authority)
	{
		/* Make noise to be picked up by PawnSensingComponent by the enemy pawns */
		MakeNoise(Loudness, this, GetActorLocation());
	}

	LastNoiseLoudness = Loudness;
	LastMakeNoiseTime = GetWorld()->GetTimeSeconds();
}


float ASCharacter::GetLastNoiseLoudness()
{
	return LastNoiseLoudness;
}


float ASCharacter::GetLastMakeNoiseTime()
{
	return LastMakeNoiseTime;
}


void ASCharacter::Suicide()
{
	KilledBy(this);
}


void ASCharacter::KilledBy(class APawn* EventInstigator)
{
	if (Role == ROLE_Authority && !bIsDying)
	{
		AController* Killer = nullptr;
		if (EventInstigator != nullptr)
		{
			Killer = EventInstigator->Controller;
			LastHitBy = nullptr;
		}

		Die(Health, FDamageEvent(UDamageType::StaticClass()), Killer, nullptr);
	}
}


void ASCharacter::OnToggleCarryActor()
{
	CarriedObjectComp->Pickup();
}


void ASCharacter::SwapToNewWeaponMesh()
{
	if (PreviousWeapon)
	{
		PreviousWeapon->AttachMeshToPawn(PreviousWeapon->GetStorageSlot());
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->AttachMeshToPawn(EInventorySlot::Hands);
	}
}

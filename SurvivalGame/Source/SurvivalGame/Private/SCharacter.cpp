// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SCharacter.h"
#include "SUsableActor.h"
#include "SCharacterMovementComponent.h"


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

	// Enable crouching
	MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;

	CameraBoomComp = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoomComp->SocketOffset = FVector(0, 35, 0);
	CameraBoomComp->TargetOffset = FVector(0, 0, 55);
	CameraBoomComp->bUsePawnControlRotation = true;
	CameraBoomComp->AttachParent = GetRootComponent();

	CameraComp = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera"));
	CameraComp->AttachParent = CameraBoomComp;

	MaxUseDistance = 800;
	bHasNewFocus = true;
	TargetingSpeedModifier = 0.5f;
	SprintingSpeedModifier = 2.5f;

	Health = 100;
	Energy = 100;
}


void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

// 	if (Role == ROLE_Authority)
// 	{
// 		Health = GetMaxHealth();
// 		Energy = GetMaxEnergy();
// 	}
}


// Called every frame
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

	// Weapons
	InputComponent->BindAction("Targeting", IE_Pressed, this, &ASCharacter::OnStartTargeting);
	InputComponent->BindAction("Targeting", IE_Released, this, &ASCharacter::OnEndTargeting);
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

	if (Controller == NULL)
		return NULL;

	Controller->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector TraceStart = CamLoc;
	const FVector Direction = CamRot.Vector();
	const FVector TraceEnd = TraceStart + (Direction * MaxUseDistance);

	FCollisionQueryParams TraceParams(FName(TEXT("TraceUsableActor")), true, this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = true;

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

void ASCharacter::SetSprinting(bool NewSprinting)
{
	bWantsToRun = NewSprinting;

	if (bIsCrouched)
		UnCrouch();

	// TODO: Stop weapon fire

	if (Role < ROLE_Authority)
	{
		ServerSetSprinting(NewSprinting);
	}
}


void ASCharacter::OnStartSprinting()
{
	SetSprinting(true);
}


void ASCharacter::OnStopSprinting()
{
	SetSprinting(false);
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

	// Don't allow sprint while strafing sideways or standing still
	return bWantsToRun && !IsTargeting() && !GetVelocity().IsZero() && (GetVelocity().GetSafeNormal2D() | GetActorRotation().Vector()) > 0.1;
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
	DOREPLIFETIME(ASCharacter, Energy);
}

void ASCharacter::OnCrouchToggle()
{
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


float ASCharacter::GetHealth() const
{
	return Health;
}


float ASCharacter::GetEnergy() const
{
	return Energy;
}


float ASCharacter::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<ASCharacter>()->Health;
}


float ASCharacter::GetMaxEnergy() const
{
	return GetClass()->GetDefaultObject<ASCharacter>()->Energy;
}


void ASCharacter::RestoreEnergy(float Amount)
{
	// Restore energy, ensure we do go outside of our bounds
	Energy = FMath::Max(Energy + Amount, GetMaxEnergy());
}


bool ASCharacter::IsAlive() const
{
	return Health > 0;
}


float ASCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (Health <= 0.f)
	{
		return 0.f;
	}

	// Temp
	return Damage;
// 
// 	// Modify damage based on gametype rules
// 	ASwitchGameGameMode* const MyGameMode = Cast<ASwitchGameGameMode>(GetWorld()->GetAuthGameMode());
// 	Damage = MyGameMode ? MyGameMode->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;
// 
// 	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
// 	if (ActualDamage > 0.f)
// 	{
// 		Health -= ActualDamage;
// 		if (Health <= 0)
// 		{
// 			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
// 		}
// 		else
// 		{
// 			PlayHit(ActualDamage, DamageEvent, EventInstigator->GetPawn(), DamageCauser, false);
// 		}
// 	}
// 
// 	return ActualDamage;
}

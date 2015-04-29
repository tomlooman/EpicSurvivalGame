// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SZombieCharacter.h"
#include "SZombieAIController.h"
#include "SCharacter.h"
#include "SBaseCharacter.h"

/* AI Include */
#include "Perception/PawnSensingComponent.h"

// Sets default values
ASZombieCharacter::ASZombieCharacter(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	/* Note: We assign the Controller class in the Blueprint extension of this class 
		Because the zombie AIController is a blueprint in content and it's better to avoid content references in code.  */
	/*AIControllerClass = ASZombieAIController::StaticClass();*/

	/* Our sensing component to detect players by visibility and noise checks. */
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(60.0f);
	PawnSensingComp->SightRadius = 2000;

	/* Ignore this channel or it will absorb the trace impacts instead of the skeletal mesh */
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f, false);
	GetCapsuleComponent()->SetCapsuleRadius(42.0f);

	/* These values are matched up to the CapsuleComponent above and are used to find navigation paths */
	GetMovementComponent()->NavAgentProps.AgentRadius = 42;
	GetMovementComponent()->NavAgentProps.AgentHeight = 192;

	Health = 75;

	/* Note: Visual Setup is done in the AI/ZombieCharacter Blueprint file */
}


void ASZombieCharacter::BeginPlay()
{
	Super::BeginPlay();

	/* This is the earliest moment we can bind our delegates */
	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &ASZombieCharacter::OnSeePlayer);
		PawnSensingComp->OnHearNoise.AddDynamic(this, &ASZombieCharacter::OnHearNoise);
	}
}


void ASZombieCharacter::OnSeePlayer(APawn* Pawn)
{
	ASZombieAIController* AIController = Cast<ASZombieAIController>(GetController());
	ASBaseCharacter* SensedPawn = Cast<ASBaseCharacter>(Pawn);
	if (AIController && SensedPawn->IsAlive())
	{
		AIController->SetTargetPlayer(Pawn);
	}

	/* Keep track of the time the player was last sensed in order to clear the target */
	LastSeenTime = GetWorld()->GetTimeSeconds();
}


void ASZombieCharacter::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, "Noise Heard!");
	}

	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), Location, 64, FColor::White, false, 1.0f);

	ASZombieAIController* AIController = Cast<ASZombieAIController>(GetController());
	if (AIController)
	{
		AIController->SetNoiseLocation(Location);
	}
}

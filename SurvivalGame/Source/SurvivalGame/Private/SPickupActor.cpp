// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SPickupActor.h"



ASPickupActor::ASPickupActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// All objects that can be picked up are simulated and can be thrown around the level
	MeshComp->SetSimulatePhysics(true);
	/* Ignore Pawn - this is to prevent objects shooting through the level or pawns glitching on top of small items. */
	MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bReplicateMovement = true;
}


void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();

	// TODO: Remove Hack to workaround constructor call not currently working.
	MeshComp->SetSimulatePhysics(true);
}


void ASPickupActor::OnUsed(APawn* InstigatorPawn)
{
	UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());

	Super::OnUsed(InstigatorPawn);
}

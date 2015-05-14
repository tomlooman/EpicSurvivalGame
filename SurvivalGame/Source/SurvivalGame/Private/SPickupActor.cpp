// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SPickupActor.h"



ASPickupActor::ASPickupActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/* Ignore Pawn - this is to prevent objects shooting through the level or pawns glitching on top of small items. */
	MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	bIsActive = false;
	bStartActive = true;
	bAllowRespawn = true;
	RespawnDelay = 5.0f;
	RespawnDelayRange = 5.0f;

	SetReplicates(true);
}


void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();

	//if (bStartActive)
	{
		RespawnPickup();
	}
}


void ASPickupActor::OnUsed(APawn* InstigatorPawn)
{
	UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());

	bIsActive = false;
	OnPickedUp();

	if (bAllowRespawn)
	{
		FTimerHandle RespawnTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ASPickupActor::RespawnPickup, RespawnDelay + FMath::RandHelper(RespawnDelayRange), false);
	}

	Super::OnUsed(InstigatorPawn);
}


void ASPickupActor::OnPickedUp()
{
	if (MeshComp)
	{
		MeshComp->SetVisibility(false);
	}
}


void ASPickupActor::RespawnPickup()
{
	bIsActive = true;
	OnRespawned();
}


void ASPickupActor::OnRespawned()
{
	if (MeshComp)
	{
		MeshComp->SetVisibility(true);
	}
}


void ASPickupActor::OnRep_IsActive()
{
	if (bIsActive)
	{
		OnRespawned();
	}
	else
	{
		OnPickedUp();
	}
}


void ASPickupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPickupActor, bIsActive);
}

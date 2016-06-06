// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SFlashlight.h"



ASFlashlight::ASFlashlight(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StorageSlot = EInventorySlot::Secondary;
	LightAttachPoint = TEXT("LightSocket");

	GetWeaponMesh()->AddLocalRotation(FRotator(0, 0, -90));

	LightConeComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightConeComp"));
	LightConeComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	LightConeComp->SetupAttachment(GetWeaponMesh(), LightAttachPoint);

	SpotLightComp = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightComp"));
	SpotLightComp->SetupAttachment(GetWeaponMesh(), LightAttachPoint);
	/*SpotLightComp->SetCastShadows(false);*/
	SpotLightComp->AddLocalRotation(FRotator(0, -90, 0));

	bIsActive = true;
	LastEmissiveStrength = -1.0f;

	EmissiveParamName = TEXT("Brightness");
	MaxEmissiveIntensity = 5.0f;

	/* Doesn't consume "Ammo" */
	StartAmmo = 0;
}


void ASFlashlight::BeginPlay()
{
	Super::BeginPlay();

	/* Create an instance unique to this actor instance to manipulate emissive intensity */
	USkeletalMeshComponent* MeshComp = GetWeaponMesh();
	if (MeshComp)
	{
		MatDynamic = MeshComp->CreateAndSetMaterialInstanceDynamic(0);
	}
}


void ASFlashlight::HandleFiring()
{
	if (Role == ROLE_Authority)
	{
		/* Toggle light,cone and material when Fired */
		bIsActive = !bIsActive;

		UpdateLight(bIsActive);
	}
}


void ASFlashlight::OnEnterInventory(ASCharacter* NewOwner)
{
	if (Role == ROLE_Authority)
	{
		bIsActive = false;

		/* Turn off light while carried on belt  */
		UpdateLight(bIsActive);
	}

	Super::OnEnterInventory(NewOwner);
}



void ASFlashlight::OnEquipFinished()
{
	Super::OnEquipFinished();

	if (Role == ROLE_Authority)
	{
		bIsActive = true;

		/* Turn off light while carried on belt  */
		UpdateLight(bIsActive);
	}
}


void ASFlashlight::OnUnEquip()
{
	Super::OnUnEquip();

	if (Role == ROLE_Authority)
	{
		bIsActive = false;

		/* Turn off light while carried on belt  */
		UpdateLight(bIsActive);
	}
}


void ASFlashlight::UpdateLight(bool Enabled)
{
	/* Turn off light while  */
	SpotLightComp->SetVisibility(Enabled);
	LightConeComp->SetVisibility(Enabled);

	/* Update material parameter */
	if (MatDynamic)
	{		
		/* " Enabled ? MaxEmissiveIntensity : 0.0f " picks between first or second value based on "Enabled" boolean */
		MatDynamic->SetScalarParameterValue(EmissiveParamName, Enabled ? MaxEmissiveIntensity : 0.0f);	
	}
}


void ASFlashlight::OnRep_IsActive()
{
	UpdateLight(bIsActive);
}


void ASFlashlight::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASFlashlight, bIsActive);
}
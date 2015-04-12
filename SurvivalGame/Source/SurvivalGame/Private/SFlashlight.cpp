// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SFlashlight.h"



ASFlashlight::ASFlashlight(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StorageSlot = EInventorySlot::Secondary;
	LightAttachPoint = TEXT("LightSocket");

	GetWeaponMesh()->AddLocalRotation(FRotator(0, 0, -90));

	// Roll -90 Roll
	// X -25

	LightConeComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightConeComp"));
	LightConeComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	LightConeComp->AttachTo(GetWeaponMesh(), LightAttachPoint, EAttachLocation::SnapToTarget);

	SpotLightComp = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightComp"));
	SpotLightComp->AttachTo(GetWeaponMesh(), LightAttachPoint, EAttachLocation::SnapToTarget);
	SpotLightComp->SetCastShadows(false);
	SpotLightComp->AddLocalRotation(FRotator(0, -90, 0));

	bIsActive = true;
	LastEmissiveStrength = -1.0f;

	EmissiveParamName = TEXT("Brightness");
	MaxEmissiveIntensity = 20.0f;
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


void ASFlashlight::FireWeapon()
{
	/* Toggle light,cone and material when Fired */
	bIsActive = !bIsActive;

	UpdateLight(bIsActive);
}


void ASFlashlight::OnEnterInventory(ASCharacter* NewOwner)
{
	/* Turn off by default */
	UpdateLight(false);

	Super::OnEnterInventory(NewOwner);
}



void ASFlashlight::OnEquipFinished()
{
	Super::OnEquipFinished();

	UpdateLight(bIsActive);
}


void ASFlashlight::OnUnEquip()
{
	Super::OnUnEquip();

	/* Turn off light while  */
	UpdateLight(false);
}


void ASFlashlight::UpdateLight(bool Enabled)
{
	/* Turn off light while  */
	SpotLightComp->SetVisibility(Enabled);
	LightConeComp->SetVisibility(Enabled);

	// TODO: Support this in multiplayer!
	
	/* Update material parameter */
	if (MatDynamic)
	{		
		/* " Enabled ? MaxEmissiveIntensity : 0.0f " picks between first or second value based on "Enabled" boolean */
		MatDynamic->SetScalarParameterValue(EmissiveParamName, Enabled ? MaxEmissiveIntensity : 0.0f);	
	}
}

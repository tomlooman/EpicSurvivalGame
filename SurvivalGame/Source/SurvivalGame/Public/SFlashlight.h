// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SWeapon.h"
#include "SFlashlight.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class SURVIVALGAME_API ASFlashlight : public ASWeapon
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

private:

	/* Material instance to manipulate emissive on the flashlight instance  */
	UMaterialInstanceDynamic* MatDynamic;
	
	UPROPERTY(EditDefaultsOnly, Category = "Material")
	FName EmissiveParamName;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
	float MaxEmissiveIntensity;

	/* Last frame's emissive intensity for performance purposes */
	float LastEmissiveStrength;

public:

	ASFlashlight(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly)
	FName LightAttachPoint;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsActive)
	bool bIsActive;

	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* LightConeComp;

	UPROPERTY(VisibleDefaultsOnly)
	USpotLightComponent* SpotLightComp;

	virtual void HandleFiring() override;

	virtual void OnEquipFinished() override;
	
	virtual void OnUnEquip() override;
	
	virtual void OnEnterInventory(ASCharacter* NewOwner);

	void UpdateLight(bool Enabled);

	UFUNCTION()
	void OnRep_IsActive();
};

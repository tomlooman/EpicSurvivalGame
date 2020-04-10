// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/SWeapon.h"
#include "SFlashlight.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class SURVIVALGAME_API ASFlashlight : public ASWeapon
{
	GENERATED_BODY()

	ASFlashlight();

	virtual void BeginPlay() override;

private:

	/* Material instance to manipulate emissive on the flashlight instance  */
	class UMaterialInstanceDynamic* MatDynamic;
	
	UPROPERTY(EditDefaultsOnly, Category = "Material")
	FName EmissiveParamName;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
	float MaxEmissiveIntensity;

	/* Last frame's emissive intensity for performance purposes */
	float LastEmissiveStrength;

public:

	UPROPERTY(EditDefaultsOnly)
	FName LightAttachPoint;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsActive)
	bool bIsActive;

	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* LightConeComp;

	UPROPERTY(VisibleDefaultsOnly)
	class USpotLightComponent* SpotLightComp;

	virtual void HandleFiring() override;

	virtual void OnEquipFinished() override;
	
	virtual void OnUnEquip() override;
	
	virtual void OnEnterInventory(ASCharacter* NewOwner);

	void UpdateLight(bool Enabled);

	UFUNCTION()
	void OnRep_IsActive();
};

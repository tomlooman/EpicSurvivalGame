// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "SDamageType.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API USDamageType : public UDamageType
{
	GENERATED_BODY()

	USDamageType();

	/* Can player die from this damage type (eg. players don't die from hunger) */
	UPROPERTY(EditDefaultsOnly)
	bool bCanDieFrom;

	/* Damage modifier for headshot damage */
	UPROPERTY(EditDefaultsOnly)
	float HeadDmgModifier;

	UPROPERTY(EditDefaultsOnly)
	float LimbDmgModifier;

public:

	bool GetCanDieFrom() const;

	float GetHeadDamageModifier() const;

	float GetLimbDamageModifier() const;
};

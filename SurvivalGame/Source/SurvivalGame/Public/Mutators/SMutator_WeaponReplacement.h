// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Mutators/SMutator.h"
#include "Items/SWeapon.h"
#include "SMutator_WeaponReplacement.generated.h"


USTRUCT(BlueprintType)
struct FReplacementInfo
{
	GENERATED_BODY()

public:

	/** class name of the weapon we want to get rid of */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<ASWeapon> FromWeapon;
	/** fully qualified path of the class to replace it with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<ASWeapon> ToWeapon;

	FReplacementInfo()
		:FromWeapon(nullptr)
		, ToWeapon(nullptr)
	{
	}

	FReplacementInfo(TSubclassOf<ASWeapon> inOldClass, TSubclassOf<ASWeapon> inNewClass)
		: FromWeapon(inOldClass)
		, ToWeapon(inNewClass)
	{
	}

};

/**
 * Allows mutators to replace weapon pickups in the active level
 */
UCLASS(ABSTRACT)
class SURVIVALGAME_API ASMutator_WeaponReplacement : public ASMutator
{
	GENERATED_BODY()

public:

	virtual void InitGame_Implementation(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual bool CheckRelevance_Implementation(AActor* Other) override;

	UPROPERTY(EditDefaultsOnly, Category = "WeaponReplacement")
	TArray<FReplacementInfo> WeaponsToReplace;
	
};

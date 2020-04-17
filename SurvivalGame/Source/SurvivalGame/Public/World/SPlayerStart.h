// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "SPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API ASPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:

	ASPlayerStart(const FObjectInitializer& ObjectInitializer);

	/* Is only useable by players - automatically a preferred spawn for players */
	UPROPERTY(EditAnywhere, Category = "PlayerStart")
	bool bPlayerOnly;

public:

	UFUNCTION(BlueprintCallable, Category = "PlayerStart")
	bool GetIsPlayerOnly() const { return bPlayerOnly; }

};

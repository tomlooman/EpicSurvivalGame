// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerStart.h"
#include "SPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API ASPlayerStart : public APlayerStart
{
	GENERATED_BODY()

	ASPlayerStart(const class FObjectInitializer& ObjectInitializer);

	/* Is only useable by players - automatically a preferred spawn for players */
	UPROPERTY(EditAnywhere, Category = "PlayerStart")
	bool bPlayerOnly;

public:

	bool GetIsPlayerOnly() { return bPlayerOnly; }

};

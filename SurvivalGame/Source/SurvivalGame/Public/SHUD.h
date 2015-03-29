// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/HUD.h"
#include "SHUD.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API ASHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

	FCanvasIcon CenterDotIcon;

	/** Main HUD update loop. */
	virtual void DrawHUD() override;
	
	void DrawCenterDot();

	/* An event hook to call HUD text events to display in the HUD. Blueprint HUD class must implement how to deal with this event. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUDEvents")
	void MessageReceived(const FString& TextMessage);
};

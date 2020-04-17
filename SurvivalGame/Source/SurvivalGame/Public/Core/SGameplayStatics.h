// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SGameplayStatics.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API USGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:


	/* Project 3D to UI variation (of engine function) that adds bTargetBehindCamera. Does NOT account for viewport DPI  */
	UFUNCTION(BlueprintPure, Category = "UI")
	static bool ProjectWorldToScreenBidirectional(APlayerController const* Player, const FVector& WorldPosition, FVector2D& ScreenPosition,
			bool& bTargetBehindCamera, bool bPlayerViewportRelative = false);

};

// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/DamageType.h"
#include "Engine/EngineTypes.h"
#include "MyBlueprintFunctionLibrary.generated.h"


/**
 * 
 */
UCLASS()
class SURVIVALGAME_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		UFUNCTION(BlueprintCallable, Category = "MyCategory")
		static FString HelloUnreal(int a, int b);

		UFUNCTION(BlueprintCallable, Category = "MyCategory")
		static FDamageEvent CreateDamageEvent();

};

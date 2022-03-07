// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "MyBlueprintFunctionLibrary.h"

FString UMyBlueprintFunctionLibrary::HelloUnreal(int a, int b)
{
	int foo = a + b;

	return (FString::Printf(TEXT("Hello Unreal, Calculated: %d + %d = %d"),a, b, foo));
}

FDamageEvent UMyBlueprintFunctionLibrary::CreateDamageEvent()
{
	TSubclassOf<UDamageType> DmgTypeClass = UDamageType::StaticClass();
	return (FDamageEvent(DmgTypeClass));
}
